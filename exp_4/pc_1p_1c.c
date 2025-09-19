#define __LIBRARY__
#include <unistd.h>
#include <stdio.h>

_syscall1(int, sem_create, char *, name);
_syscall2(int, sem_set, int, semid, int, value);
_syscall1(int, sem_wait, int, semid);
_syscall1(int, sem_signal, int, semid);
_syscall1(int, pc_insert, int, item);
_syscall0(int, pc_delete);

#define NR_BUFFERS 3
#define NR_PRODUCES 10
#define NR_CONSUMES 10

void producer(int mutex, int empty, int full)
{
    int i;
    for (i = 0; i < NR_PRODUCES; i++)
    {
        sem_wait(empty);
        sem_wait(mutex);

        pc_insert(i);
        printf("Producer %d: PRODUCE a item=%d\n", getpid(), i);
        fflush(stdout);

        sem_signal(mutex);
        sem_signal(full);
    }
}

void consumer(int mutex, int empty, int full)
{
    int i, item;
    for (i = 0; i < NR_CONSUMES; i++)
    {
        sem_wait(full);
        sem_wait(mutex);

        item = pc_delete();
        printf("Consumer %d: CONSUME a item=%d\n", getpid(), item);
        fflush(stdout);

        sem_signal(mutex);
        sem_signal(empty);
    }
}

int main()
{
    int mutex, empty, full;

    printf("This is a Demo of Producer-Consumer problem\n");
    fflush(stdout);

    mutex = sem_create("PCMutex");
    sem_set(mutex, 1);

    empty = sem_create("PCEmpty");
    sem_set(empty, NR_BUFFERS);

    full = sem_create("PCFull");
    sem_set(full, 0);

    printf("The number of Producer-Consumer buffers is: %d\n", NR_BUFFERS);
    fflush(stdout);

    if (!fork())
    {
        producer(mutex, empty, full);
        return 0;
    }

    if (!fork())
    {
        consumer(mutex, empty, full);
        return 0;
    }

    wait(NULL);
    wait(NULL);

    printf("Producer-Consumer Demo Finished\n");
    fflush(stdout);

    return 0;
}