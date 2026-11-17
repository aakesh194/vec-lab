// 05_deleted_benchmark.c — The compiler can delete work whose result nobody sees.
// Run: ./run.sh 5
#include <stdio.h>
#include <time.h>

#define N 100000000L

static double now(void) {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec + t.tv_nsec / 1e9;
}

int main(void) {
    // Version 1: result is never used -> dead code, the compiler may remove it.
    double s = now();
    long unused = 0;
    for (long i = 0; i < N; i++) unused += i * i % 7;
    double e = now();
    printf("result NOT used: %.4f s\n", e - s);

    // Version 2: same loop, but we print the result -> must actually run.
    s = now();
    long used = 0;
    for (long i = 0; i < N; i++) used += i * i % 7;
    e = now();
    printf("result used:     %.4f s   (value %ld)\n", e - s, used);
    return 0;
}
