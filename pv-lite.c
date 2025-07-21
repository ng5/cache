#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define BUF_SIZE (1024 * 1024) // 1 MiB

int main() {
    char *buf = malloc(BUF_SIZE);
    if (!buf) {
        perror("malloc");
        return 1;
    }

    size_t total_bytes = 0;
    ssize_t n;

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    while ((n = read(STDIN_FILENO, buf, BUF_SIZE)) > 0) {
        total_bytes += n;
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    free(buf);

    if (n < 0) {
        perror("read");
        return 1;
    }

    double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) * 1e-9;
    double mib = total_bytes / (1024.0 * 1024.0);
    double rate = mib / elapsed;

    fprintf(stderr, "Read %.2f MiB in %.6f sec = %.2f MiB/s\n", mib, elapsed, rate);
    return 0;
}
