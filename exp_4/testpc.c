// 文件: testpc.c

#define __LIBRARY__
#include <unistd.h>
#include <stdio.h>

// 定义系统调用接口
_syscall1(int, sem_create, char*, name);
_syscall2(int, sem_set, int, semid, int, value);
_syscall1(int, sem_wait, int, semid);
_syscall1(int, sem_signal, int, semid);

#define NR_BUFFERS 3
#define NR_PRODUCES 10
#define NR_CONSUMES 10

int buffer[NR_BUFFERS];
int head = 0; // 消费者取数据的位置
int tail = 0; // 生产者放数据的位置

void producer() {
    int i;
    int mutex, empty, full;

    // 获取信号量
    mutex = sem_create("PCMutex");
    empty = sem_create("PCEmpty");
    full = sem_create("PCFull");

    for (i = 0; i < NR_PRODUCES; i++) {
        sem_wait(empty);
        sem_wait(mutex);

        buffer[tail] = i;
        printf("Producer %d: PRODUCE a item=%d into buffer[%d]\n", getpid(), i, tail);
        tail = (tail + 1) % NR_BUFFERS;

        sem_signal(mutex);
        sem_signal(full);
    }
}

void consumer() {
    int i, item;
    int mutex, empty, full;

    // 获取信号量
    mutex = sem_create("PCMutex");
    empty = sem_create("PCEmpty");
    full = sem_create("PCFull");
    
    for (i = 0; i < NR_CONSUMES; i++) {
        sem_wait(full);
        sem_wait(mutex);

        item = buffer[head];
        printf("Consumer %d: CONSUME a item=%d from buffer[%d]\n", getpid(), item, head);
        head = (head + 1) % NR_BUFFERS;

        sem_signal(mutex);
        sem_signal(empty);
    }
}

int main() {
    int mutex, empty, full;

    printf("------This is a Demo of Producer-Consumer problem------\n");

    // 创建并初始化信号量
    mutex = sem_create("PCMutex");
    sem_set(mutex, 1);

    empty = sem_create("PCEmpty");
    sem_set(empty, NR_BUFFERS);

    full = sem_create("PCFull");
    sem_set(full, 0);

    printf("The number of Producer-Consumer buffers is: %d\n", NR_BUFFERS);
    
    if (!fork()) {
        // 子进程 - 生产者
        producer();
        return 0;
    }

    if (!fork()) {
        // 子进程 - 消费者
        consumer();
        return 0;
    }

    // 父进程等待两个子进程结束
    wait(NULL);
    wait(NULL);

    return 0;
}