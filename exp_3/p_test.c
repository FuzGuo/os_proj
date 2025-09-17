#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/times.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#define HZ 100
#define NUM_PROCESSES 5
#define TOTAL_DURATION 20
#define PRIME_SEARCH_LIMIT 500


void cpu_work_task()
{
    int i, j, is_prime;
    for (i = 2; i <= PRIME_SEARCH_LIMIT; i++) {
        is_prime = 1;
        for (j = 2; j < i; j++) {
            if (i % j == 0) {
                is_prime = 0;
                break;
            }
        }
    }
}


void io_work_task()
{
    sleep(1);
}


void run_simulation(int total_time, int cpu_slice, int io_slice)
{
    struct tms time_start, time_now;
    clock_t cpu_consumed;
    int remaining_time = total_time;

    while (remaining_time > 0) {
        if (cpu_slice > 0) {
            times(&time_start);
            do {
                cpu_work_task();
                times(&time_now);
                cpu_consumed = (time_now.tms_utime - time_start.tms_utime) +
                               (time_now.tms_stime - time_start.tms_stime);
            } while ((cpu_consumed / HZ) < cpu_slice);
            remaining_time -= (cpu_consumed / HZ);
        }

        if (remaining_time <= 0) break;

        if (io_slice > 0) {
            io_work_task();
            remaining_time -= io_slice;
        }
    }
}


pid_t create_worker(int total_time, int cpu_slice, int io_slice)
{
    pid_t pid = fork();

    if (pid < 0) {
        return -1;
    }

    if (pid == 0) {
        printf("  Child (PID %d) started by Parent (PID %d)\n", getpid(), getppid());
        run_simulation(total_time, cpu_slice, io_slice);
        printf("  Child (PID %d) finished.\n", getpid());
        exit(0);
    }
    
    return pid;
}

int main(void)
{
    pid_t pids[NUM_PROCESSES];
    int i;

    printf("Main process (PID %d) starting test...\n", getpid());

 
    pids[0] = create_worker(TOTAL_DURATION, 1, 9); 
    pids[1] = create_worker(TOTAL_DURATION, 3, 7);
    pids[2] = create_worker(TOTAL_DURATION, 5, 5); 
    pids[3] = create_worker(TOTAL_DURATION, 8, 2);
    pids[4] = create_worker(TOTAL_DURATION, 10, 0);

    for (i = 0; i < NUM_PROCESSES; i++) {
        wait(NULL);
    }
    
    printf("All child processes have completed.\n");
    printf("Main process (PID %d) finished.\n", getpid());

    return 0;
}