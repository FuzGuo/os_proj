// 文件: kernel/sem.c
// 描述: 重构后的信号量和生产者-消费者缓冲区实现。
// API接口保持原样，内部逻辑已用更健壮的版本替换。

#define __LIBRARY__
#include <linux/sched.h>
#include <linux/kernel.h>
#include <asm/segment.h>
#include <asm/system.h>
#include <string.h>
#include <unistd.h>
#include <errno.h> // 包含了错误码定义，如 EINVAL, ENOSPC, EAGAIN

/* --- 新的、更健壮的内部定义 --- */

#define NR_SEMS 30          // 系统支持的信号量最大数量
#define MAX_SEM_NAME 20     // 信号量名字的最大长度
#define MAX_SEM_WAIT 10     // 每个信号量最多允许等待的进程数

// 新的信号量数据结构
struct semaphore {
    char s_name[MAX_SEM_NAME + 1]; // 信号量的名字
    int ref_cnt;                   // 引用计数，为0时表示空闲
    int s_value;                   // 信号量的值
    struct task_struct *s_wait[MAX_SEM_WAIT]; // 等待该信号量的进程队列 (数组实现)
};

// 全局信号量数组
static struct semaphore SemaphoreSet[NR_SEMS];

#define PC_BUFFER_SIZE 3 // 共享缓冲区的大小 (来自您原始代码的定义)
#define PC_BUFFER_MAX (PC_BUFFER_SIZE)

// 新的生产者-消费者缓冲区数据结构
struct PCBuffer {
    int max_size;
    int push_ptr; // 生产者指针 (tail)
    int pop_ptr;  // 消费者指针 (head)
    /* 额外一个元素用于区分满/空状态 */
    int buffer[PC_BUFFER_MAX + 1];
} pc_buffer;


/* --- 内部辅助函数 (来自正确版本) --- */

// 比较用户空间字符串和内核空间字符串是否相等
static int get_fs_str_eq(const char *user_space_str, const char *sys_space_str) {
    char c;
    int i;
    for (i = 0; i < MAX_SEM_NAME; i++) {
        c = get_fs_byte(user_space_str + i);
        if (c != sys_space_str[i]) {
            return 0;
        }
        if (c == '\0') {
            break;
        }
    }
    return 1;
}

// 从用户空间拷贝字符串到内核空间
static int get_fs_str_cpy(char *sys_space_str, const char *user_space_str) {
    char c;
    int i;
    for (i = 0; i < MAX_SEM_NAME; i++) {
        c = get_fs_byte(user_space_str + i);
        sys_space_str[i] = c;
        if (c == '\0') {
            break;
        }
    }
    // 确保字符串以空字符结尾
    sys_space_str[MAX_SEM_NAME] = '\0';
    return i;
}

// 计算循环缓冲区的下一个指针位置
static int next_ptr(int ptr) {
    return (ptr + 1) % (pc_buffer.max_size + 1);
}


/* --- 系统启动时的初始化函数 --- */

void sem_init(void) {
    int i;
    // 初始化所有信号量，将引用计数设为0表示未使用
    for (i = 0; i < NR_SEMS; i++) {
        SemaphoreSet[i].ref_cnt = 0;
    }

    // 初始化生产者-消费者缓冲区
    pc_buffer.max_size = PC_BUFFER_SIZE;
    pc_buffer.push_ptr = 0;
    pc_buffer.pop_ptr = 0;
    
    printk("Semaphore mechanism and PC Buffer initialized.\n");
}


/* --- API 实现 (保持接口不变, 替换内部逻辑) --- */

// 系统调用: 创建或获取一个信号量
int sys_sem_create(char * name) {
    int i, j;
    struct semaphore *sem;

    cli(); // 关中断，保证原子性

    // 1. 查找是否已存在同名信号量
    for (i = 0; i < NR_SEMS; i++) {
        sem = &SemaphoreSet[i];
        if (sem->ref_cnt > 0 && get_fs_str_eq(name, sem->s_name)) {
            sem->ref_cnt++;
            sti();
            return i; // 找到同名信号量，增加引用计数并返回ID
        }
    }

    // 2. 如果不存在，则查找一个空闲位置创建新的
    for (i = 0; i < NR_SEMS; i++) {
        sem = &SemaphoreSet[i];
        if (sem->ref_cnt == 0) {
            sem->ref_cnt = 1;
            get_fs_str_cpy(sem->s_name, name);
            sem->s_value = 0; // 遵循原API行为，创建时默认值为0
            // 初始化等待队列
            for (j = 0; j < MAX_SEM_WAIT; j++) {
                sem->s_wait[j] = NULL;
            }
            sti();
            return i; // 创建成功，返回ID
        }
    }

    sti();
    return -ENOSPC; // 没有空闲信号量，返回错误 (No space left on device)
}

// 系统调用: 设置信号量的值
int sys_sem_set(int semid, int value) {
    if (semid < 0 || semid >= NR_SEMS || SemaphoreSet[semid].ref_cnt == 0) {
        return -EINVAL; // 无效ID或信号量未使用
    }
    SemaphoreSet[semid].s_value = value;
    return value;
}

// 系统调用: P操作 (wait)
int sys_sem_wait(int semid) {
    struct semaphore *s;
    cli();

    if (semid < 0 || semid >= NR_SEMS || SemaphoreSet[semid].ref_cnt == 0) {
        sti();
        return -EINVAL;
    }
    s = &SemaphoreSet[semid];
    
    s->s_value--;
    
    // 如果值小于0，说明需要等待
    if (s->s_value < 0) {
        // 检查等待队列是否已满
        if (-s->s_value > MAX_SEM_WAIT) {
            s->s_value++; // 操作回滚
            sti();
            return -EAGAIN; // 等待队列已满，返回错误 (Try again)
        }
        printk("Process %d SLEEPON semaphore %s (value: %d)\n", current->pid, s->s_name, s->s_value);
        // 在等待队列的相应位置上睡眠
        sleep_on(&s->s_wait[-s->s_value - 1]);
    }

    sti();
    return 0;
}

// 系统调用: V操作 (signal)
int sys_sem_signal(int semid) {
    struct semaphore *s;
    cli();
    
    if (semid < 0 || semid >= NR_SEMS || SemaphoreSet[semid].ref_cnt == 0) {
        sti();
        return -EINVAL;
    }
    s = &SemaphoreSet[semid];

    s->s_value++;
    
    // 如果值小于等于0，说明有进程在等待，需要唤醒一个
    if (s->s_value <= 0) {
        printk("Process %d WAKEUP a process on semaphore %s (value: %d)\n", current->pid, s->s_name, s->s_value);
        // 唤醒等待队列中的一个进程
        wake_up(&s->s_wait[-s->s_value]);
        s->s_wait[-s->s_value] = NULL; // 清空该等待槽位
    }

    sti();
    return 0;
}

// 系统调用: 生产者向缓冲区放入数据
int sys_pc_insert(int item) {
    cli();
    // 检查缓冲区是否已满
    if (next_ptr(pc_buffer.push_ptr) == pc_buffer.pop_ptr) {
        sti();
        // 用户空间的 'empty' 信号量应该阻止这种情况发生
        // 但为安全起见，返回错误
        return -EAGAIN;
    }
    pc_buffer.buffer[pc_buffer.push_ptr] = item;
    pc_buffer.push_ptr = next_ptr(pc_buffer.push_ptr);
    sti();
    return 0;
}

// 系统调用: 消费者从缓冲区取出数据
int sys_pc_delete(void) {
    int item;
    cli();
    // 检查缓冲区是否为空
    if (pc_buffer.pop_ptr == pc_buffer.push_ptr) {
        sti();
        // 用户空间的 'full' 信号量应该阻止这种情况发生
        // 但为安全起见，返回错误
        return -EAGAIN;
    }
    item = pc_buffer.buffer[pc_buffer.pop_ptr];
    pc_buffer.pop_ptr = next_ptr(pc_buffer.pop_ptr);
    sti();
    return item;
}