#define _GNU_SOURCE
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <immintrin.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/perf_event.h>
#include <asm/unistd.h>

#define ITERS 100000000

long perf_event_open(struct perf_event_attr *hw_event, pid_t pid,
                     int cpu, int group_fd, unsigned long flags) {
    return syscall(__NR_perf_event_open, hw_event, pid, cpu, group_fd, flags);
}

int main() {
    struct perf_event_attr pe = {
        .type = PERF_TYPE_HARDWARE,
        .size = sizeof(struct perf_event_attr),
        .config = PERF_COUNT_HW_INSTRUCTIONS,
        .disabled = 1,
        .exclude_kernel = 1,
        .exclude_hv = 1
    };
    int fd_instr = perf_event_open(&pe, 0, -1, -1, 0);
    if (fd_instr == -1) perror("perf_event_open instructions");

    pe.config = PERF_COUNT_HW_CPU_CYCLES;
    int fd_cycles = perf_event_open(&pe, 0, -1, -1, 0);
    if (fd_cycles == -1) perror("perf_event_open cycles");

    __m256 a = _mm256_set1_ps(1.0f);
    __m256 b = _mm256_set1_ps(2.0f);
    __m256 acc0 = _mm256_setzero_ps();
    __m256 acc1 = _mm256_setzero_ps();
    __m256 acc2 = _mm256_setzero_ps();
    __m256 acc3 = _mm256_setzero_ps();

    ioctl(fd_instr, PERF_EVENT_IOC_RESET, 0);
    ioctl(fd_cycles, PERF_EVENT_IOC_RESET, 0);
    ioctl(fd_instr, PERF_EVENT_IOC_ENABLE, 0);
    ioctl(fd_cycles, PERF_EVENT_IOC_ENABLE, 0);

    for (int i = 0; i < ITERS; ++i) {
        acc0 = _mm256_fmadd_ps(a, b, acc0);
        acc1 = _mm256_fmadd_ps(a, b, acc1);
        acc2 = _mm256_fmadd_ps(a, b, acc2);
        acc3 = _mm256_fmadd_ps(a, b, acc3);
    }

    ioctl(fd_instr, PERF_EVENT_IOC_DISABLE, 0);
    ioctl(fd_cycles, PERF_EVENT_IOC_DISABLE, 0);

    long long instrs = 0, cycles = 0;
    read(fd_instr, &instrs, sizeof(instrs));
    read(fd_cycles, &cycles, sizeof(cycles));

    __m256 sum01 = _mm256_add_ps(acc0, acc1);
    __m256 sum23 = _mm256_add_ps(acc2, acc3);
    __m256 total = _mm256_add_ps(sum01, sum23);

    float result[8];
    _mm256_storeu_ps(result, total);
    float sum = 0.0f;
    for (int i = 0; i < 8; ++i) sum += result[i];

    printf("Sum: %.1f\n", sum);
    printf("Instructions: %lld\n", instrs);
    printf("Cycles:       %lld\n", cycles);
    printf("IPC:          %.2f\n", (double)instrs / cycles);

    close(fd_instr);
    close(fd_cycles);
    return 0;
}

