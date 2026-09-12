#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
TARGET="${1:-bench_dce}"
FILTER="${2:-.*}"

for BUILD_TYPE in Debug RelWithDebInfo Release; do
  BUILD_DIR="${ROOT}/build-${BUILD_TYPE}"
  if command -v ninja >/dev/null 2>&1; then
    cmake -S "${ROOT}" -B "${BUILD_DIR}" -DCMAKE_BUILD_TYPE="${BUILD_TYPE}" -G Ninja
  else
    cmake -S "${ROOT}" -B "${BUILD_DIR}" -DCMAKE_BUILD_TYPE="${BUILD_TYPE}"
  fi
  cmake --build "${BUILD_DIR}"
  echo
  echo "========== ${BUILD_TYPE} =========="
  "${BUILD_DIR}/${TARGET}" --benchmark_filter="${FILTER}" --benchmark_repetitions=3 \
    --benchmark_report_aggregates_only=true
done
