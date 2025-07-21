#define _GNU_SOURCE
#include <pthread.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#define NUM_THREADS 5
#define ITERATIONS 1000000

typedef struct {
  long value;
  char padding[64 - sizeof(long)];
} PaddedData;

PaddedData data[NUM_THREADS];

void *worker(void *arg) {
  int id = *(int *)arg;
  for (long i = 0; i < ITERATIONS; ++i) {
    data[id].value++;
  }
  return NULL;
}

int main() {
  pthread_t threads[NUM_THREADS];
  int ids[NUM_THREADS];

  for (int i = 0; i < NUM_THREADS; ++i) {
    ids[i] = i;
    pthread_create(&threads[i], NULL, worker, &ids[i]);
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(i, &cpuset); // wrap if threads > cores

    pthread_setaffinity_np(threads[i], sizeof(cpu_set_t), &cpuset);
  }

  for (int i = 0; i < NUM_THREADS; ++i) {
    pthread_join(threads[i], NULL);
  }

  for (int i = 0; i < NUM_THREADS; ++i) {
    printf("Thread %d: %ld\n", i, data[i].value);
  }

  return 0;
}
