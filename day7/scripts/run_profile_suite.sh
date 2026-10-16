#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BUILD="${ROOT}/build"
LINES="${1:-300000}"
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

echo "========== demo slow/fast =========="
run "${BUILD}/mini_grep_demo" "${LINES}" both ERROR

if command -v perf >/dev/null 2>&1; then
  echo
  echo "========== perf stat slow =========="
  run perf stat -e cycles,instructions,cache-references,cache-misses,branches,branch-misses \
    -- "${BUILD}/mini_grep_demo" "${LINES}" slow ERROR

  echo
  echo "========== perf stat fast =========="
  run perf stat -e cycles,instructions,cache-references,cache-misses,branches,branch-misses \
    -- "${BUILD}/mini_grep_demo" "${LINES}" fast ERROR

  DATA="${ROOT}/perf.data"
  rm -f "${DATA}"
  echo
  echo "========== perf record slow =========="
  run perf record -g -F 997 -o "${DATA}" -- "${BUILD}/mini_grep_demo" "${LINES}" slow ERROR
  perf report -i "${DATA}" --stdio --no-children | head -35
else
  echo "perf not available; skip stat/record"
fi

if command -v heaptrack >/dev/null 2>&1; then
  echo
  echo "========== heaptrack slow (optional, slow) =========="
  heaptrack -o "${ROOT}/heaptrack.slow.gz" "${BUILD}/mini_grep_demo" "${LINES}" slow ERROR
  echo "Saved ${ROOT}/heaptrack.slow.gz"
fi
