#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BUILD="${ROOT}/build"
cmake -S "${ROOT}" -B "${BUILD}" -DCMAKE_BUILD_TYPE=RelWithDebInfo
cmake --build "${BUILD}"
for F in 'SumBranch/random' 'SumBranchless/random' 'SumBranch/mostly_pass'; do
  echo "========== ${F} =========="
  perf stat -e branches,branch-misses,cycles,instructions \
    -- "${BUILD}/bench_branch" --benchmark_filter="${F}" --benchmark_min_time=0.3s 2>&1 | tail -12
done
