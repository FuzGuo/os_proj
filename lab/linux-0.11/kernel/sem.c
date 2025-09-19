#define __LIBRARY__
#include <linux/sched.h>
#include <linux/kernel.h>
#include <asm/segment.h>
#include <asm/system.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#define NR_SEMS 30
#define MAX_SEM_NAME 20
#define MAX_SEM_WAIT 10

struct semaphore {
    char s_name[MAX_SEM_NAME + 1];
    int ref_cnt;
    int s_value;
    struct task_struct *s_wait[MAX_SEM_WAIT];
};

static struct semaphore SemaphoreSet[NR_SEMS];

#define PC_BUFFER_SIZE 3
#define PC_BUFFER_MAX (PC_BUFFER_SIZE)

struct PCBuffer {
    int max_size;
    int push_ptr;
    int pop_ptr;
    int buffer[PC_BUFFER_MAX + 1];
} pc_buffer;

static int next_ptr(int ptr) {
    return (ptr + 1) % (pc_buffer.max_size + 1);
}

void sem_init(void) {
    int i;
    for (i = 0; i < NR_SEMS; i++) {
        SemaphoreSet[i].ref_cnt = 0;
    }

    pc_buffer.max_size = PC_BUFFER_SIZE;
    pc_buffer.push_ptr = 0;
    pc_buffer.pop_ptr = 0;
    
    printk("Semaphore mechanism and PC Buffer initialized.\n");
}

int sys_sem_create(char * name) {
    int i, j, k;
    struct semaphore *sem;
    char c;

    cli();

    for (i = 0; i < NR_SEMS; i++) {
        sem = &SemaphoreSet[i];
        if (sem->ref_cnt > 0) {
            int is_equal = 1;
            for (k = 0; k < MAX_SEM_NAME; k++) {
                c = get_fs_byte(name + k);
                if (c != sem->s_name[k]) {
                    is_equal = 0;
                    break;
                }
                if (c == '\0') {
                    break;
                }
            }
            if (is_equal) {
                sem->ref_cnt++;
                sti();
                return i;
            }
        }
    }

    for (i = 0; i < NR_SEMS; i++) {
        sem = &SemaphoreSet[i];
        if (sem->ref_cnt == 0) {
            sem->ref_cnt = 1;
            
            for (k = 0; k < MAX_SEM_NAME; k++) {
                c = get_fs_byte(name + k);
                sem->s_name[k] = c;
                if (c == '\0') {
                    break;
                }
            }
            sem->s_name[MAX_SEM_NAME] = '\0';

            sem->s_value = 0;
            for (j = 0; j < MAX_SEM_WAIT; j++) {
                sem->s_wait[j] = NULL;
            }
            sti();
            return i;
        }
    }

    sti();
    return -ENOSPC;
}

int sys_sem_set(int semid, int value) {
    if (semid < 0 || semid >= NR_SEMS || SemaphoreSet[semid].ref_cnt == 0) {
        return -EINVAL;
    }
    SemaphoreSet[semid].s_value = value;
    return value;
}

int sys_sem_wait(int semid) {
    struct semaphore *s;
    cli();

    if (semid < 0 || semid >= NR_SEMS || SemaphoreSet[semid].ref_cnt == 0) {
        sti();
        return -EINVAL;
    }
    s = &SemaphoreSet[semid];
    
    s->s_value--;
    
    if (s->s_value < 0) {
        if (-s->s_value > MAX_SEM_WAIT) {
            s->s_value++;
            sti();
            return -EAGAIN;
        }
        printk("Process %d SLEEPON semaphore %s (value: %d)\n", current->pid, s->s_name, s->s_value);
        sleep_on(&s->s_wait[-s->s_value - 1]);
    }

    sti();
    return 0;
}

int sys_sem_signal(int semid) {
    struct semaphore *s;
    cli();
    
    if (semid < 0 || semid >= NR_SEMS || SemaphoreSet[semid].ref_cnt == 0) {
        sti();
        return -EINVAL;
    }
    s = &SemaphoreSet[semid];

    s->s_value++;
    
    if (s->s_value <= 0) {
        printk("Process %d WAKEUP a process on semaphore %s (value: %d)\n", current->pid, s->s_name, s->s_value);
        wake_up(&s->s_wait[-s->s_value]);
        s->s_wait[-s->s_value] = NULL;
    }

    sti();
    return 0;
}

int sys_pc_insert(int item) {
    cli();
    if (next_ptr(pc_buffer.push_ptr) == pc_buffer.pop_ptr) {
        sti();
        return -EAGAIN;
    }
    pc_buffer.buffer[pc_buffer.push_ptr] = item;
    pc_buffer.push_ptr = next_ptr(pc_buffer.push_ptr);
    sti();
    return 0;
}

int sys_pc_delete(void) {
    int item;
    cli();
    if (pc_buffer.pop_ptr == pc_buffer.push_ptr) {
        sti();
        return -EAGAIN;
    }
    item = pc_buffer.buffer[pc_buffer.pop_ptr];
    pc_buffer.pop_ptr = next_ptr(pc_buffer.pop_ptr);
    sti();
    return item;
}