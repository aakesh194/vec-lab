#!/usr/bin/env bash
# Usage: ./run.sh        (all experiments)
#        ./run.sh 3      (just experiment 3)
# Works with gcc (Linux) and clang (macOS, where "gcc" is really clang).

CC=${CC:-gcc}
cd "$(dirname "$0")"
mkdir -p bin

# Is the compiler really clang? (true on macOS)
if $CC --version 2>&1 | grep -qi clang; then
  IS_CLANG=1
  VEC_YES="-Rpass=loop-vectorize"
  VEC_NO="-Rpass-missed=loop-vectorize -Rpass-analysis=loop-vectorize"
  NOVEC="-fno-vectorize -fno-slp-vectorize"
else
  IS_CLANG=0
  VEC_YES="-fopt-info-vec-optimized"
  VEC_NO="-fopt-info-vec-missed"
  NOVEC="-fno-tree-vectorize"
fi

# On x86, let it use the widest SIMD your CPU has (AVX2 = 8 floats).
# On Apple Silicon (arm64) SIMD is 128-bit NEON (4 floats); no flag needed.
ARCH=""
[ "$(uname -m)" = "x86_64" ] && ARCH="-march=native"

hdr() { echo; echo "================ $1 ================"; }
want() { [ -z "$ONLY" ] || [ "$ONLY" = "$1" ]; }
ONLY=$1

if want 1; then
  hdr "1. Optimization levels"
  for O in O0 O1 O2 O3; do
    $CC -$O $ARCH 01_levels.c -o bin/levels_$O && printf "%-4s " "-$O" && ./bin/levels_$O
  done
  echo "-- same -O3, vectorization switched OFF:"
  $CC -O3 $ARCH $NOVEC 01_levels.c -o bin/levels_novec && printf "%-4s " "-O3*" && ./bin/levels_novec
fi

if want 2; then
  hdr "2. Which loops vectorize? (compiler report)"
  echo "-- VECTORIZED:"
  $CC -O3 $ARCH -c $VEC_YES 02_which_loops.c -o /dev/null 2>&1 | grep -i "02_which_loops"
  echo "-- NOT vectorized (and why):"
  $CC -O3 $ARCH -c $VEC_NO 02_which_loops.c -o /dev/null 2>&1 | grep -i "02_which_loops" | grep -v "^ " | head -15
fi

if want 3; then
  hdr "3. Aliasing and restrict"
  echo "-- compiler report (look at the two add_ functions):"
  $CC -O3 $ARCH -c $VEC_YES 03_aliasing.c -o /dev/null 2>&1 | grep -i "03_aliasing" | grep -iE "vector|version|alias"
  $CC -O3 $ARCH 03_aliasing.c -o bin/alias && ./bin/alias
fi

if want 4; then
  hdr "4. Float vs int sums"
  echo "-- report at plain -O3:"
  $CC -O3 $ARCH -c $VEC_YES 04_float_sum.c -o /dev/null 2>&1 | grep -i "04_float_sum"
  $CC -O3 $ARCH 04_float_sum.c -o bin/sum && ./bin/sum
  echo "-- report with -ffast-math:"
  $CC -O3 $ARCH -ffast-math -c $VEC_YES 04_float_sum.c -o /dev/null 2>&1 | grep -i "04_float_sum"
  $CC -O3 $ARCH -ffast-math 04_float_sum.c -o bin/sum_fast && ./bin/sum_fast
fi

if want 5; then
  hdr "5. The deleted benchmark"
  for O in O0 O2; do
    echo "-- -$O:"
    $CC -$O 05_deleted_benchmark.c -o bin/dead_$O && ./bin/dead_$O
  done
fi
echo
