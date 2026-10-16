#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BUILD="${ROOT}/build"
KIND="${1:-row}"
N="${2:-1024}"
REPEAT="${3:-8}"
CPU="${TASKSET_CPU:-2}"

cmake -S "${ROOT}" -B "${BUILD}" -DCMAKE_BUILD_TYPE=RelWithDebInfo
cmake --build "${BUILD}"

EVENTS="cycles,instructions,cache-references,cache-misses,branches,branch-misses"

RUN=( perf stat -d -e "${EVENTS}" -- "${BUILD}/workload_demo" "${N}" "${KIND}" "${REPEAT}" )

if command -v taskset >/dev/null 2>&1; then
  taskset -c "${CPU}" "${RUN[@]}"
else
  "${RUN[@]}"
fi

echo
echo "Tip: IPC = instructions/cycles; compare row vs col cache-miss rate; branchy vs sorted data (Day 11)."
