#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BUILD="${ROOT}/build"
CPU="${TASKSET_CPU:-2}"

if command -v ninja >/dev/null 2>&1; then
  cmake -S "${ROOT}" -B "${BUILD}" -G Ninja
else
  cmake -S "${ROOT}" -B "${BUILD}"
fi
cmake --build "${BUILD}"

RUN=( "${BUILD}/bench_traverse" --benchmark_repetitions=3
      --benchmark_report_aggregates_only=true )

if command -v taskset >/dev/null 2>&1; then
  taskset -c "${CPU}" "${RUN[@]}"
else
  "${RUN[@]}"
fi
