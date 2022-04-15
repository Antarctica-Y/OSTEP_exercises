#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

u_int64_t rdtsc() {
    unsigned int lo, hi;
    __asm__ volatile("rdtsc" : "=a"(lo), "=d"(hi));
    return ((u_int64_t)hi << 32) | lo;
}

int main(int argc, char *argv[]) {
    u_int64_t start, end;
    int fd = open("./test.txt", O_RDWR);

    char buffer[10];

    start = rdtsc();
    for (int i = 0; i < 1000; i++) {
        read(fd, buffer, 0);
    }
    end = rdtsc();

    printf("%lu\n", (end - start));

    return 0;
}