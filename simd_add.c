// compile with: gcc -O3 -mavx2 -march=native -o simd_add simd_add.c
#include <immintrin.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define N 1024 * 1024

float src1[N] __attribute__((aligned(32)));
float src2[N] __attribute__((aligned(32)));
float dst[N]   __attribute__((aligned(32)));

int main() {
    // Initialize inputs
    for (int i = 0; i < N; ++i) {
        src1[i] = i * 1.0f;
        src2[i] = i * 2.0f;
    }

for (int repeat = 0; repeat < 100000; ++repeat)
    for (int i = 0; i < N; i += 8) {
        __m256 a = _mm256_load_ps(&src1[i]);
        __m256 b = _mm256_load_ps(&src2[i]);
        __m256 c = _mm256_add_ps(a, b);
        _mm256_store_ps(&dst[i], c);
    }



    // Check result
    printf("dst[42] = %.1f\n", dst[42]);
    return 0;
}

