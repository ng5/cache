// Compile with: gcc -O3 -mavx2 -march=native -o simd_bench simd_bench.c
#include <immintrin.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <x86intrin.h>

#define N 1024 * 1024
#define REPEATS 100

float src1[N] __attribute__((aligned(32)));
float src2[N] __attribute__((aligned(32)));
float dst[N] __attribute__((aligned(32)));

uint64_t rdtsc_start() {
  unsigned int dummy;
  _mm_lfence(); // Serialize
  return __rdtsc();
}

uint64_t rdtsc_end() {
  _mm_lfence(); // Serialize
  return __rdtsc();
}

int main() {
  // Warm up memory (prevents page faults)
  for (int i = 0; i < N; ++i)
    src1[i] = 1.0f;
  for (int i = 0; i < N; ++i)
    src2[i] = 2.0f;

  uint64_t start = rdtsc_start();
  float result = 0.0f;
  for (int r = 0; r < REPEATS; ++r) {
    for (int i = 0; i < N; i += 8) {
      __m256 a = _mm256_load_ps(&src1[i]);
      __m256 b = _mm256_load_ps(&src2[i]);
      __m256 c = _mm256_add_ps(a, b);
      _mm256_store_ps(&dst[i], c);
    }
  }

  for (int i = 0; i < N; i++)
    result += dst[i]; // force load
  printf("dst[42] = %.1f, checksum = %.1f\n", dst[42], result);

  uint64_t end = rdtsc_end();
  uint64_t cycles = end - start;

  // Assume fixed frequency for simplicity (e.g., 3.5 GHz)
  double freq_ghz = 3.5;
  double seconds = cycles / (freq_ghz * 1e9);
  double bytes_moved =
      (double)N * 3 * sizeof(float) * REPEATS; // 2 reads + 1 write
  double gbps = bytes_moved / (1e9 * seconds);

  printf("Cycles: %lu\n", cycles);
  printf("Time: %.6f sec\n", seconds);
  printf("Throughput: %.2f GB/s\n", gbps);
  printf("Result sample: dst[42] = %.1f\n", dst[42]);

  return 0;
}
