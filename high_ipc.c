#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <immintrin.h> // For AVX2 intrinsics

// Function to perform a horizontal sum of double-precision floats in a __m256d vector
// This is not a single instruction in AVX2, but a common pattern.
static inline double sum_m256d(__m256d v) {
    // Permute to move the higher elements into lower positions for summation
    __m128d sum128 = _mm256_extractf128_pd(v, 0); // Get lower 128 bits
    __m128d sum256 = _mm256_extractf128_pd(v, 1); // Get upper 128 bits

    sum128 = _mm_add_pd(sum128, sum256); // Add the two 128-bit halves

    // SSE3 had HADD. For SSE2/Scalar, often involves extract/shift/add.
    // _mm_hadd_pd is available in SSE3/SSSE3
    sum128 = _mm_hadd_pd(sum128, sum128); // Horizontal add: [a+b, c+d] becomes [a+b+c+d, a+b+c+d]

    return _mm_cvtsd_f64(sum128); // Extract the final double-precision sum
}


// This function performs a series of vectorized operations using AVX2 intrinsics
// to maximize instructions per cycle, specifically for double-precision floating points.
// It processes 64-bit floating points in parallel (4 per AVX2 register).
uint64_t high_ipc_example_avx2_fp(int iterations) {
    // Initialize 256-bit double-precision floating point vectors. Each can hold 4 x double.
    // _mm256_set_pd(d3, d2, d1, d0) sets elements from highest to lowest index.
    __m256d vec_a = _mm256_set_pd(3.0, 2.0, 1.0, 0.0);
    __m256d vec_b = _mm256_set_pd(7.0, 6.0, 5.0, 4.0);
    __m256d vec_c = _mm256_set_pd(11.0, 10.0, 9.0, 8.0);
    __m256d vec_d = _mm256_set_pd(15.0, 14.0, 13.0, 12.0);

    for (int i = 0; i < iterations; ++i) {
        // Perform multiple independent AVX2 floating-point operations
        vec_a = _mm256_add_pd(vec_a, vec_b); // a = a + b (packed double add)
        vec_b = _mm256_sub_pd(vec_b, vec_c); // b = b - c (packed double subtract)
        vec_c = _mm256_mul_pd(vec_c, vec_d); // c = c * d (packed double multiply)
        vec_d = _mm256_div_pd(vec_d, vec_a); // d = d / a (packed double divide - slower than others)

        // More operations to keep the pipeline full
        vec_a = _mm256_blend_pd(vec_a, vec_d, 0b1010); // Conditional blend
        vec_b = _mm256_add_pd(vec_b, vec_c);
        vec_c = _mm256_sub_pd(vec_c, vec_a);
        vec_d = _mm256_mul_pd(vec_d, vec_b);
    }

    // Sum all elements across all vectors to ensure the computations are not optimized away.
    double final_sum_double = 0.0;
    final_sum_double += sum_m256d(vec_a);
    final_sum_double += sum_m256d(vec_b);
    final_sum_double += sum_m256d(vec_c);
    final_sum_double += sum_m256d(vec_d);

    // Return as uint64_t for consistency with previous examples' return type
    // (though the value itself might be large or fractional).
    return (uint64_t)final_sum_double;
}

int main() {
    int iterations = 100000000; // 100 million vector iterations

    printf("Starting high IPC AVX2 FP example with %d vector iterations...\n", iterations);
    printf("Each vector iteration performs 8 x 4 = 32 equivalent double scalar operations.\n");
    printf("Total equivalent scalar operations will be ~%lld\n", (long long)iterations * 8 * 4);

    clock_t start_time = clock();

    uint64_t final_sum_avx2_fp = high_ipc_example_avx2_fp(iterations);

    clock_t end_time = clock();
    double time_taken = (double)(end_time - start_time) / CLOCKS_PER_SEC;

    printf("Execution finished.\n");
    printf("Time taken: %f seconds\n", time_taken);
    printf("Final observable sum (from AVX2 FP - cast to uint64_t): %llu\n", final_sum_avx2_fp);

    return 0;
}
