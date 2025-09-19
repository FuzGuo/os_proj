#define __LIBRARY__
#include <unistd.h>
#include <stdio.h>

_syscall1(int, pc_insert, int, item);
_syscall0(int, pc_delete);

#define TARGET_PRODUCE_COUNT 20
#define TARGET_CONSUME_COUNT 20

void producer()
{
    int produced_count = 0;
    unsigned long fail_count = 0;
    int item = 0;

    printf("Producer %d started, target is %d items.\n", getpid(), TARGET_PRODUCE_COUNT);
    fflush(stdout);

    while (produced_count < TARGET_PRODUCE_COUNT)
    {
        if (pc_insert(item) == 0)
        {
            printf("Producer %d: SUCCESS producing item %d\n", getpid(), item);
            fflush(stdout);
            produced_count++;
            item++;
        }
        else
        {
            fail_count++;
            if (fail_count % 100000 == 0)
            {
                printf("Producer %d: insert failed, retry count: %lu\n", getpid(), fail_count);
                fflush(stdout);
            }
        }
    }
    printf(" Producer %d Finished , Success: %d, Failures: %lu\n", getpid(), produced_count, fail_count);
    fflush(stdout);
}

void consumer()
{
    int consumed_count = 0;
    unsigned long fail_count = 0;
    int item;

    printf("Consumer %d started, target is %d items.\n", getpid(), TARGET_CONSUME_COUNT);
    fflush(stdout);

    while (consumed_count < TARGET_CONSUME_COUNT)
    {
        item = pc_delete();
        if (item >= 0)
        {
            printf("Consumer %d: SUCCESS consuming item %d\n", getpid(), item);
            fflush(stdout);
            consumed_count++;
        }
        else
        {
            fail_count++;
            if (fail_count % 100000 == 0)
            {
                printf("Consumer %d: delete failed, retry count: %lu\n", getpid(), fail_count);
                fflush(stdout);
            }
        }
    }
    printf(" Consumer %d Finished, Success: %d, Failures: %lu\n", getpid(), consumed_count, fail_count);
    fflush(stdout);
}

int main()
{
    printf(" Demo without Semaphores \n");
    fflush(stdout);

    if (!fork())
    {
        producer();
        return 0;
    }

    if (!fork())
    {
        consumer();
        return 0;
    }

    wait(NULL);
    wait(NULL);

    printf(" Demo without Semaphores Finished \n");
    fflush(stdout);
    return 0;
}