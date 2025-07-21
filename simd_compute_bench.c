// simd_compute_bench.c
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <immintrin.h>

#define SIZE   (1 << 14)   // 16K floats = 64 KB per array
#define REPEAT (1 << 13)   // 8192 reps = compute-heavy

float src1[SIZE] __attribute__((aligned(32)));
float src2[SIZE] __attribute__((aligned(32)));
float dst_scalar[SIZE] __attribute__((aligned(32)));
float dst_simd[SIZE]   __attribute__((aligned(32)));
volatile float result_scalar = 0.0f;
volatile float result_simd = 0.0f;

double now() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

void scalar_add() {
    for (int r = 0; r < REPEAT; r++) {
        for (size_t i = 0; i < SIZE; i++) {
            dst_scalar[i] = src1[i]*2.0f + src2[i]*3.0f + dst_scalar[i]*1.1f;
        }
    }
    for (size_t i = 0; i < SIZE; i++) result_scalar += dst_scalar[i];
}

void simd_add() {
    __m256 mul2 = _mm256_set1_ps(2.0f);
    __m256 mul3 = _mm256_set1_ps(3.0f);
    __m256 mul1_1 = _mm256_set1_ps(1.1f);
    for (int r = 0; r < REPEAT; r++) {
        for (size_t i = 0; i < SIZE; i += 8) {
            __m256 a = _mm256_load_ps(&src1[i]);
            __m256 b = _mm256_load_ps(&src2[i]);
            __m256 c = _mm256_load_ps(&dst_simd[i]);

            __m256 t1 = _mm256_mul_ps(a, mul2);
            __m256 t2 = _mm256_mul_ps(b, mul3);
            __m256 t3 = _mm256_mul_ps(c, mul1_1);
            __m256 sum = _mm256_add_ps(t1, _mm256_add_ps(t2, t3));

            _mm256_store_ps(&dst_simd[i], sum);
        }
    }
    for (size_t i = 0; i < SIZE; i++) result_simd += dst_simd[i];
}

int main() {
    for (size_t i = 0; i < SIZE; i++) {
        src1[i] = 1.0f;
        src2[i] = 2.0f;
        dst_scalar[i] = dst_simd[i] = 3.0f;
    }

    double t1 = now();
    scalar_add();
    double t2 = now();

    double t3 = now();
    simd_add();
    double t4 = now();

    double scalar_time = t2 - t1;
    double simd_time = t4 - t3;
    double flops = (double)SIZE * REPEAT * 5; // 5 FLOPs per element

    printf("Result Scalar: %.2f\n", result_scalar);
    printf("Result SIMD:   %.2f\n", result_simd);
    printf("Scalar time:   %.6f sec\n", scalar_time);
    printf("SIMD time:     %.6f sec\n", simd_time);
    printf("Speedup:       %.2fx\n", scalar_time / simd_time);
    printf("Scalar GFLOP/s: %.2f\n", flops / scalar_time / 1e9);
    printf("SIMD   GFLOP/s: %.2f\n", flops / simd_time / 1e9);
    return 0;
}

