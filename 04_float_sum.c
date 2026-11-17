// 04_float_sum.c — Float sums don't vectorize by default; int sums do.
// Run: ./run.sh 4
#include <stdio.h>
#include <time.h>

#define N 4096
#define REPS 100000

float fa[N];
int   ia[N];

static double now(void) {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec + t.tv_nsec / 1e9;
}

float sum_float(void) {
    float s = 0.0f;
    for (int i = 0; i < N; i++) s += fa[i];
    return s;
}

int sum_int(void) {
    int s = 0;
    for (int i = 0; i < N; i++) s += ia[i];
    return s;
}

int main(void) {
    // Values chosen so rounding happens: big and tiny numbers mixed.
    for (int i = 0; i < N; i++) {
        fa[i] = (i % 2) ? 1e8f : 1.0f;
        ia[i] = i;
    }

    volatile float fsink = 0;   // volatile: forces the result to "matter"
    double t0 = now();
    for (int r = 0; r < REPS; r++) { fa[r % N] += 0.0f; fsink = sum_float(); }
    double t1 = now();

    volatile int isink = 0;
    for (int r = 0; r < REPS; r++) { ia[r % N] += 0; isink = sum_int(); }
    double t2 = now();

    printf("float sum = %.1f   time %.3f s\n", (double)fsink, t1 - t0);
    printf("int   sum = %d   time %.3f s\n", isink, t2 - t1);
    return 0;
}
