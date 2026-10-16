#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BUILD="${ROOT}/build"
CPU="${TASKSET_CPU:-2}"

cmake -S "${ROOT}" -B "${BUILD}" -DCMAKE_BUILD_TYPE=RelWithDebInfo
cmake --build "${BUILD}"

run() {
  if command -v taskset >/dev/null 2>&1; then
    taskset -c "${CPU}" "$@"
  else
    "$@"
  fi
}

echo "========== benchmark =========="
run "${BUILD}/bench_blur" --benchmark_repetitions=3 --benchmark_report_aggregates_only=true

if command -v perf >/dev/null 2>&1; then
  echo
  echo "========== perf stat naive =========="
  run perf stat -e cache-references,cache-misses,cycles,instructions \
    -- "${BUILD}/bench_blur" --benchmark_filter=BM_Naive --benchmark_min_time=0.5s 2>&1 | tail -15
  echo
  echo "========== perf stat separable =========="
  run perf stat -e cache-references,cache-misses,cycles,instructions \
    -- "${BUILD}/bench_blur" --benchmark_filter=BM_Separable --benchmark_min_time=0.5s 2>&1 | tail -15
fi
