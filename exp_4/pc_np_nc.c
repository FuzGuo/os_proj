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
#define N_PRODUCERS 3
#define N_CONSUMERS 3
#define ITEMS_PER_PRODUCER 4
#define ITEMS_PER_CONSUMER 4

void producer(int mutex, int empty, int full)
{
    int i;
    for (i = 0; i < ITEMS_PER_PRODUCER; i++)
    {
        int item = getpid() * 100 + i;

        sem_wait(empty);
        sem_wait(mutex);

        pc_insert(item);
        printf("Producer %d: PRODUCE a item=%d\n", getpid(), item);
        fflush(stdout);

        sem_signal(mutex);
        sem_signal(full);
    }
}

void consumer(int mutex, int empty, int full)
{
    int i, item;
    for (i = 0; i < ITEMS_PER_CONSUMER; i++)
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
    int i;
    int total_processes = N_PRODUCERS + N_CONSUMERS;

    printf(" Demo with Semaphores (%d Producers, %d Consumers) \n", N_PRODUCERS, N_CONSUMERS);
    fflush(stdout);

    mutex = sem_create("PCMutex_Multi");
    sem_set(mutex, 1);

    empty = sem_create("PCEmpty_Multi");
    sem_set(empty, NR_BUFFERS);

    full = sem_create("PCFull_Multi");
    sem_set(full, 0);

    for (i = 0; i < N_PRODUCERS; i++)
    {
        if (!fork())
        {
            producer(mutex, empty, full);
            return 0;
        }
    }

    for (i = 0; i < N_CONSUMERS; i++)
    {
        if (!fork())
        {
            consumer(mutex, empty, full);
            return 0;
        }
    }

    for (i = 0; i < total_processes; i++)
    {
        wait(NULL);
    }

    printf(" Demo (%dP/%dC) Finished \n", N_PRODUCERS, N_CONSUMERS);
    fflush(stdout);

    return 0;
}