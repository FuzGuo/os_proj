// 文件: kernel/sem.c

#define __LIBRARY__
#include <unistd.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <asm/segment.h>
#include <string.h>
#include <asm/system.h>  // <--- 添加这一行


#define NR_SEMS 30 // 系统支持的信号量最大数量

// 信号量数据结构
struct semaphore {
    char s_name[20];            // 信号量的名字
    int s_value;                // 信号量的值
    int s_used;                 // 该信号量是否被使用 (1=used, 0=free)
    struct task_struct * s_wait; // 等待该信号量的进程队列
};

// 全局信号量数组
static struct semaphore SemaphoreSet[NR_SEMS];

// 初始化所有信号量 (在系统启动时调用)
void sem_init(void) {
    int i;
    for (i = 0; i < NR_SEMS; i++) {
        SemaphoreSet[i].s_name[0] = '\0';
        SemaphoreSet[i].s_value = 0;
        SemaphoreSet[i].s_used = 0;
        SemaphoreSet[i].s_wait = NULL;
    }
    printk("Semaphore mechanism initialized.\n");
}

// 系统调用: 创建或获取一个信号量
// 参数: name - 信号量名字
// 返回: 成功则返回信号量ID(数组下标), 失败返回-1
int sys_sem_create(char * name) {
    char tmp_name[20];
    int i, free_slot = -1;

    // 从用户空间拷贝名字
    for (i = 0; i < 20; i++) {
        tmp_name[i] = get_fs_byte(name + i);
        if (tmp_name[i] == '\0') break;
    }
    if (i == 20) tmp_name[19] = '\0'; // 防止越界

    // 查找是否已存在同名信号量
    for (i = 0; i < NR_SEMS; i++) {
        if (SemaphoreSet[i].s_used && strcmp(tmp_name, SemaphoreSet[i].s_name) == 0) {
            return i; // 找到同名信号量，直接返回ID
        }
        if (!SemaphoreSet[i].s_used && free_slot == -1) {
            free_slot = i; // 记录第一个空闲位置
        }
    }

    // 如果没有找到同名信号量，则使用找到的空闲位置创建一个新的
    if (free_slot != -1) {
        strcpy(SemaphoreSet[free_slot].s_name, tmp_name);
        SemaphoreSet[free_slot].s_value = 0;
        SemaphoreSet[free_slot].s_used = 1;
        SemaphoreSet[free_slot].s_wait = NULL;
        return free_slot;
    }
    
    // 没有空闲位置，创建失败
    return -1;
}

// 系统调用: 设置信号量的值 (通常用于初始化)
// 参数: semid - 信号量ID, value - 初始值
// 返回: 成功则返回设置后的值, 失败返回-1
int sys_sem_set(int semid, int value) {
    if (semid < 0 || semid >= NR_SEMS || !SemaphoreSet[semid].s_used) {
        return -1; // 无效ID
    }
    SemaphoreSet[semid].s_value = value;
    return value;
}

// 系统调用: P操作 (wait)
// 参数: semid - 信号量ID
// 返回: 成功返回0, 失败返回-1
int sys_sem_wait(int semid) {
    cli(); // 关中断，保证操作原子性
    if (semid < 0 || semid >= NR_SEMS || !SemaphoreSet[semid].s_used) {
        sti();
        return -1;
    }

    SemaphoreSet[semid].s_value--;
    if (SemaphoreSet[semid].s_value < 0) {
        // 需要睡眠等待
        printk("Process %d SLEEPON semaphore %s\n", current->pid, SemaphoreSet[semid].s_name);
        sleep_on(&SemaphoreSet[semid].s_wait);
    }
    sti(); // 开中断
    return 0;
}

// 系统调用: V操作 (signal)
// 参数: semid - 信号量ID
// 返回: 成功返回0, 失败返回-1
int sys_sem_signal(int semid) {
    cli(); // 关中断
    if (semid < 0 || semid >= NR_SEMS || !SemaphoreSet[semid].s_used) {
        sti();
        return -1;
    }
    
    SemaphoreSet[semid].s_value++;
    if (SemaphoreSet[semid].s_value <= 0) {
        // 有进程在等待，唤醒一个
        printk("Process %d WAKEUP a process on semaphore %s\n", current->pid, SemaphoreSet[semid].s_name);
        wake_up(&SemaphoreSet[semid].s_wait);
    }
    sti(); // 开中断
    return 0;
}