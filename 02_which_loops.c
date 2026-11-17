// 02_which_loops.c — Which loops can the compiler vectorize?
// Nothing to run: you only read the compiler's report.
// Run: ./run.sh 2
// Each loop is in its own function so the report's line numbers are easy to match.

#define N 1024
float a[N + 8], b[N], c[N];

// LOOP A: independent iterations -> SHOULD vectorize
void loop_a_independent(void) {
    for (int i = 0; i < N; i++)
        c[i] = a[i] + b[i];
}

// LOOP B: reads the value the previous iteration just wrote -> should NOT vectorize
void loop_b_dependency(void) {
    for (int i = 1; i < N; i++)
        a[i] = a[i - 1] + 1.0f;
}

// LOOP C: reads AHEAD (a[i+4]) -> SHOULD vectorize (reads see original values)
void loop_c_read_ahead(void) {
    for (int i = 0; i < N; i++)
        a[i] = a[i + 4] * 2.0f;
}

// LOOP D: reads 4 BEHIND -> real dependency at distance 4.
// Compiler may vectorize with only 4 lanes, or refuse. Check the report!
void loop_d_read_behind_4(void) {
    for (int i = 4; i < N; i++)
        a[i] = a[i - 4] * 2.0f;
}
