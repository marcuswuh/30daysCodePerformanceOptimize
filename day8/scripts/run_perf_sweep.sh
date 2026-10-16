#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BUILD="${ROOT}/build"
CPU="${TASKSET_CPU:-2}"

cmake -S "${ROOT}" -B "${BUILD}" -DCMAKE_BUILD_TYPE=RelWithDebInfo
cmake --build "${BUILD}"

for N in 64 256 1024 2048; do
  echo "========== n=${N} row =========="
  if command -v taskset >/dev/null 2>&1; then
    taskset -c "${CPU}" perf stat -e cache-references,cache-misses,cycles,instructions \
      -- "${BUILD}/bench_cache" --benchmark_filter="RowMajor/${N}" --benchmark_min_time=0.2s 2>&1 | tail -20
    echo "========== n=${N} col =========="
    taskset -c "${CPU}" perf stat -e cache-references,cache-misses,cycles,instructions \
      -- "${BUILD}/bench_cache" --benchmark_filter="ColMajor/${N}" --benchmark_min_time=0.2s 2>&1 | tail -20
  else
    perf stat -e cache-references,cache-misses -- "${BUILD}/bench_cache" \
      --benchmark_filter="RowMajor/${N}" --benchmark_min_time=0.2s 2>&1 | tail -15
  fi
done
