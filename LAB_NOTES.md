# Vectorization lab (COMP 211 checkoff)

## How to run

```bash
cd vec-lab
chmod +x run.sh
./run.sh          # all 5 experiments
./run.sh 2        # just one
CC=clang ./run.sh # use clang instead of gcc
```

- **macOS:** `gcc` is actually clang, and the script detects that and uses clang's report flags. Apple Silicon has 128-bit SIMD, so you'll see width 4 instead of 8.
- **Timings vary by machine.** Look at the *ratios* between runs, not the exact numbers. Run each experiment twice.

The sample numbers below came from an x86 Linux machine with AVX2 (8 floats per register).

---

## 1. Optimization levels: `01_levels.c`
**Tests:** the same `c[i] = a[i] + b[i]` loop compiled at `-O0` through `-O3`, plus `-O3` with vectorization turned off.

**Sample:**
```
-O0  1.154 s
-O1  0.289 s
-O2  0.044 s
-O3  0.054 s
-O3 (no vectorization) 0.373 s
```
**What to notice:**
- `-O0` to `-O1`: about 4× faster just from keeping variables in registers instead of memory.
- `-O1` to `-O2`/`-O3`: another big jump, mostly from vectorization.
- **`-O3` vs `-O3` without vectorization is about 7×.** This is the cleanest proof: same optimization level, only SIMD toggled. That's close to the theoretical 8× for 8 lanes.
- `-O2` vs `-O3` is roughly a tie. Higher isn't automatically faster.

## 2. Which loops vectorize: `02_which_loops.c`
**Tests:** 4 loops; you just read the compiler's report.

| Loop | Code | Expect | Why |
|---|---|---|---|
| A (line 11) | `c[i] = a[i] + b[i]` | vectorized, 8 wide | iterations independent |
| B (line 17) | `a[i] = a[i-1] + 1` | **NOT** vectorized | needs the value just written |
| C (line 23) | `a[i] = a[i+4] * 2` | vectorized, 8 wide | reads ahead, so it sees original values |
| D (line 30) | `a[i] = a[i-4] * 2` | vectorized, **only 4 wide** | dependency distance is 4, so 8 lanes would be wrong |

**What to notice:** loop D. The compiler drops to 16-byte vectors (gcc) or `width: 4` (clang). It works out *exactly* how far apart the dependency is and uses the widest SIMD that's still safe.

## 3. Aliasing: `03_aliasing.c`
**Tests:** `add` with and without `restrict`, then calls the non-restrict one with overlapping pointers.

**What to notice:**
- gcc prints `loop versioned for vectorization because of possible aliasing` for the non-restrict version. That means it built two copies of the loop (SIMD and scalar) plus a runtime check to pick between them. The `restrict` version doesn't need that.
- The overlapping call prints `1 2 4 8 16 ...`, which is correct because the runtime check caught the overlap. Blindly vectorized code would give `1 2 2 2 ...`.
- **Takeaway:** without `restrict` you pay for a check and extra code. With `restrict` you skip it, but if you break the promise, the behavior is undefined.

## 4. Float vs int sums: `04_float_sum.c`
**Tests:** summing floats vs ints, with and without `-ffast-math`.

**Sample (gcc):**
```
plain -O3:   float sum = 204805013504.0  0.220 s   | int 0.012 s
-ffast-math: float sum = 204799524864.0  0.029 s   | int 0.017 s
```
**What to notice:**
- **The float sum changes** with `-ffast-math`. That's rounding: adding in a different order gives a different answer, because float addition isn't associative.
- The float sum is about **7× faster** with `-ffast-math`. The int sum is fast either way, since int addition is associative and the compiler can always reorder it.
- **gcc quirk:** gcc's report claims the float loop is vectorized even at plain `-O3`. It loads 8 floats at a time but still adds them **one by one in order** (`vaddss`, the scalar add, 8 times). That's why the result matches `-O0` and it's barely faster. Clang is more honest and just says `cannot prove it is safe to reorder floating-point operations`.

## 5. The deleted benchmark: `05_deleted_benchmark.c`
**Tests:** the same loop twice, once with the result unused and once with it printed.

**Sample:**
```
-O0:  NOT used 0.23 s   used 0.23 s
-O2:  NOT used 0.0000 s used 0.088 s
```
**What to notice:** at `-O2` the unused loop takes **zero time** because the compiler deleted it (dead code elimination, under the as-if rule: nobody can observe the result). That's why every other benchmark here prints a value or uses `volatile`.

---

## Extra things to try
- In `02`, change loop D to `a[i-8]` or `a[i-2]` and see how the vector width changes.
- In `01`, raise `N` to `10000000`. The speedup shrinks because the arrays no longer fit in cache and RAM becomes the bottleneck.
- Paste `01_levels.c` into **godbolt.org** with `-O3 -march=native`. You'll see `vaddps` and `ymm` registers in the SIMD version and `vaddss`/`xmm` without it.
- Remove `restrict` from `01_levels.c` and check whether the timing changes. The runtime check is cheap, so it usually doesn't change much.
