#define _GNU_SOURCE /* See feature_test_macros(7) */
#define errExit(msg)        \
    do {                    \
        perror(msg);        \
        exit(EXIT_FAILURE); \
    } while (0)

#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

u_int64_t rdtsc() {
    u_int64_t ret;

    asm("rdtsc; shl $32, %%rdx; or %%rdx, %%rax;"
        : "=A"(ret)
        : /* no input */
        : "%edx");

    return ret;
}

int main(int argc, char *argv[]) {
    int pipeA[2];
    int pipeB[2];

    pid_t wpid;
    int status = 0;

    cpu_set_t set;
    int cpu_id = 0;
    CPU_ZERO(&set); // 初始化

    if (pipe(pipeA) < 0) exit(1);
    if (pipe(pipeB) < 0) exit(1);

    int child1 = fork();
    if (child1 < 0) {
        fprintf(stderr, "fork child1 failed\n");
        exit(1);
    } else if (child1 == 0) {
        CPU_SET(cpu_id, &set); // 将cpu_id加入到set中
        // 绑定cpu
        if (sched_setaffinity(getpid(), sizeof(set), &set) == -1) errExit("sched_setaffinity");

        close(pipeA[0]);
        close(pipeB[1]);

        char *msg = "";
        char buffer[1];
        u_int64_t start, end;

        start = rdtsc();
        for (int i = 0; i < 1000; ++i) {
            write(pipeA[1], msg, 0);
            read(pipeB[0], msg, 0);
        }
        end = rdtsc();

        printf("child1 cpu_id:%d,  end - start:%ld\n", sched_getcpu(), end - start);

        return 0;
    }

    int child2 = fork();

    if (child2 < 0) {
        fprintf(stderr, "fork child2 failed\n");
        exit(1);
    } else if (child2 == 0) {
        CPU_SET(cpu_id, &set);
        if (sched_setaffinity(getpid(), sizeof(set), &set) == -1) errExit("sched_setaffinity");

        close(pipeA[1]);
        close(pipeB[0]);

        char *msg = "";
        char buffer[1];

        u_int64_t start, end;

        start = rdtsc();
        for (int i = 0; i < 1000; ++i) {
            read(pipeA[0], msg, 0);
            write(pipeB[1], msg, 0);
        }
        end = rdtsc();

        printf("child2 cpu_id:%d,  end - start:%ld\n", sched_getcpu(), end - start);

        return 0;
    }

    while ((wpid = wait(&status)) > 0)
        ;
    printf("parent pid: %d\n", (int)getpid());

    return 0;
}
