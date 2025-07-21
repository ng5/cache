#include <stdio.h>
#include <stdint.h>
#include <x86intrin.h>

#define ITERS 100000000

int main() {
    volatile uint64_t a0 = 1, a1 = 2, a2 = 3, a3 = 4;
    volatile uint64_t b0 = 5, b1 = 6, b2 = 7, b3 = 8;
    uint64_t acc0 = 0, acc1 = 0, acc2 = 0, acc3 = 0;

    unsigned long long start = __rdtsc();

    for (int i = 0; i < ITERS; ++i) {
        acc0 += a0 * b0 + i;
        acc1 += a1 + b1 * i;
        acc2 += a2 ^ b2 + i;
        acc3 += a3 + b3 ^ i;
    }

    unsigned long long end = __rdtsc();

    uint64_t total = acc0 + acc1 + acc2 + acc3;
    printf("Sum: %llu\n", total);
    printf("Cycles: %llu\n", end - start);
    return 0;
}

