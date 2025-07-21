#include <immintrin.h>
#include <stdio.h>
#include <x86intrin.h>

#define ITERS 10000000

int main() {
    __m256 a = _mm256_set1_ps(1.0f);
    __m256 b = _mm256_set1_ps(2.0f);
    __m256 acc0 = _mm256_setzero_ps();
    __m256 acc1 = _mm256_setzero_ps();
    __m256 acc2 = _mm256_setzero_ps();
    __m256 acc3 = _mm256_setzero_ps();

    unsigned long long start = __rdtsc();

    for (int i = 0; i < ITERS; ++i) {
        acc0 = _mm256_fmadd_ps(a, b, acc0);
        acc1 = _mm256_fmadd_ps(a, b, acc1);
        acc2 = _mm256_fmadd_ps(a, b, acc2);
        acc3 = _mm256_fmadd_ps(a, b, acc3);
    }

    unsigned long long end = __rdtsc();

    __m256 sum01 = _mm256_add_ps(acc0, acc1);
    __m256 sum23 = _mm256_add_ps(acc2, acc3);
    __m256 total = _mm256_add_ps(sum01, sum23);

    float result[8];
    _mm256_storeu_ps(result, total);
    float sum = 0.0f;
    for (int i = 0; i < 8; ++i) sum += result[i];

    printf("Sum: %.1f\n", sum);
    printf("Cycles: %llu\n", end - start);
    return 0;
}

