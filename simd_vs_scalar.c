#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <immintrin.h>

#define SIZE (1 << 15)  // 256K floats = 1MB per array
#define REPEAT 100000
float src1[SIZE] __attribute__((aligned(32)));
float src2[SIZE] __attribute__((aligned(32)));
float dst_scalar[SIZE] __attribute__((aligned(32)));
float dst_simd[SIZE] __attribute__((aligned(32)));

double now() {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec + t.tv_nsec * 1e-9;
}

void scalar_add() {
    float sum = 0.0f;
    for (int r = 0; r < REPEAT; r++) {
        for (size_t i = 0; i < SIZE; ++i)
            dst_scalar[i] = src1[i] + src2[i];
	sum += dst_scalar[r % SIZE];
    }
    printf("sum=%f\n",sum);
}

void simd_add() {
    float sum = 0.0f;
    for (int r = 0; r < REPEAT; r++) {
        for (size_t i = 0; i < SIZE; i += 8) {
            __m256 a = _mm256_load_ps(&src1[i]);
            __m256 b = _mm256_load_ps(&src2[i]);
            __m256 c = _mm256_add_ps(a, b);
            _mm256_store_ps(&dst_simd[i], c);
        }
	sum += dst_simd[r % SIZE];
    }
    printf("sum=%f\n",sum);
}

float checksum(const float* arr) {
    float sum = 0.0f;
    for (size_t i = 0; i < SIZE; ++i)
        sum += arr[i];
    return sum;
}

int main() {
    for (size_t i = 0; i < SIZE; ++i) {
        src1[i] = 1.0f;
        src2[i] = 2.0f;
    }

    double t1 = now();
    scalar_add();
    double t2 = now();

    double t3 = now();
    simd_add();
    double t4 = now();

    float c1 = checksum(dst_scalar);
    float c2 = checksum(dst_simd);

    printf("Scalar checksum: %.1f\n", c1);
    printf("SIMD checksum:   %.1f\n", c2);
    printf("Scalar time: %.6f sec\n", t2 - t1);
    printf("SIMD time:   %.6f sec\n", t4 - t3);
    printf("Speedup:     %.2fx\n", (t2 - t1) / (t4 - t3));
}

