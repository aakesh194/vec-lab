# vec-lab
 
My COMP 211 topic exploration on compiler optimizations and vectorization. (Aakesh Yoganathan)
 
Vectorization is when the compiler rewrites a loop so one instruction adds 8 numbers at once instead of 1, using wide SIMD registers. I wanted to see how much it actually speeds things up and when the compiler won't do it.
 
## What I found
 
- **-O3 wasn't faster than -O2.** -O0 took 1.154 s, -O1 0.289 s, -O2 0.044 s, -O3 0.054 s. -O2 already vectorized the loop, so -O3 made the same code.
- **Vectorization alone gave about 7x.** At -O3 with vectorization turned off (`-fno-tree-vectorize`) it took 0.373 s, vs 0.054 s with it on. The max with 8 lanes is 8x.
- **Iterations have to be independent.** `c[i] = a[i] + b[i]` vectorizes, but `a[i] = a[i-1] + 1` doesn't, since each step needs the one before it.
- **The compiler can delete your benchmark.** A loop whose result was never printed ran in 0 seconds at -O2.
Timings are from a sample run on an x86 machine with AVX2. Yours will be different.
 
## Running it
 
Needs `gcc` or `clang`.
 
```bash
chmod +x run.sh
./run.sh      # all 5 experiments
./run.sh 1    # just one (1-5)
```
 
| File | What it tests |
|---|---|
| `01_levels.c` | the same loop at -O0 to -O3, plus -O3 with vectorization off |
| `02_which_loops.c` | which loops the compiler can vectorize |
| `03_aliasing.c` | what `restrict` does |
| `04_float_sum.c` | float vs int sums, with and without `-ffast-math` |
| `05_deleted_benchmark.c` | unused results getting deleted |
 
More detail on each one is in `LAB_NOTES.md`. On a Mac, `gcc` is really clang and the script handles it. Apple Silicon fits 4 floats per register instead of 8, so the speedup will be smaller.
 
## Slides
 
In `slides/`.
 
## Resources
 
- [GCC optimization options](https://gcc.gnu.org/onlinedocs/gcc/Optimize-Options.html)
- [GCC auto-vectorization](https://gcc.gnu.org/projects/tree-ssa/vectorization.html)
- [Compiler Explorer](https://godbolt.org/)
- My AI conversation: <https://claude.ai/share/4bb4ee5b-6935-44cc-8ae8-e88b293d55d4>