// 01_levels.c — How much do -O0/-O1/-O2/-O3 speed up the same loop?
// Run: ./run.sh 1
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 4096      // small enough to stay in cache, so we measure compute, not RAM
#define REPS 200000

static double now(void) {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec + t.tv_nsec / 1e9;
}

void add(float *restrict a, float *restrict b, float *restrict c, int n) {
    for (int i = 0; i < n; i++)
        c[i] = a[i] + b[i];
}

int main(void) {
    float *a = malloc(N * sizeof *a), *b = malloc(N * sizeof *b), *c = malloc(N * sizeof *c);
    for (int i = 0; i < N; i++) { a[i] = i; b[i] = 2.0f * i; }

    double s = now();
    for (int r = 0; r < REPS; r++) {
        add(a, b, c, N);
        a[r % N] += 1.0f;   // tiny change each rep so the compiler can't skip repeats
    }
    double e = now();

    // Printing c makes the result "observable", so the loop can't be deleted.
    printf("time: %.3f s   (check: c[100] = %.1f)\n", e - s, c[100]);
    free(a); free(b); free(c);
    return 0;
}
