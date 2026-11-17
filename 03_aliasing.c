// 03_aliasing.c — Why pointers can block vectorization, and what restrict does.
// Run: ./run.sh 3
#include <stdio.h>

// No restrict: the compiler must assume a, b, c MIGHT overlap.
void add_maybe_alias(float *a, float *b, float *c, int n) {
    for (int i = 0; i < n; i++)
        c[i] = a[i] + b[i];
}

// restrict: you promise they don't overlap.
void add_restrict(float *restrict a, float *restrict b, float *restrict c, int n) {
    for (int i = 0; i < n; i++)
        c[i] = a[i] + b[i];
}

int main(void) {
    // Overlapping call: c points ONE slot past a, so writing c[i] changes a[i+1].
    float x[17];
    for (int i = 0; i < 17; i++) x[i] = 1.0f;

    add_maybe_alias(x, x, x + 1, 16);   // c[i] = a[i] + a[i]  where c = a+1

    printf("overlapping call (correct, scalar-style answer):\n");
    for (int i = 0; i < 17; i++) printf("%g ", x[i]);
    printf("\n\nEach value doubles the previous one: 1 2 4 8 ...\n");
    printf("That is a hidden dependency. If the compiler had vectorized blindly,\n");
    printf("it would have read 8 old 1's at once and gotten 1 2 2 2 2 ... (wrong).\n");
    printf("It stayed correct because it checked for overlap at runtime.\n");
    return 0;
}
