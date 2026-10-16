#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BUILD="${ROOT}/build"
KIND="${1:-col}"
N="${2:-1024}"
REPEAT="${3:-12}"
CPU="${TASKSET_CPU:-2}"
DATA="${ROOT}/perf.data"

cmake -S "${ROOT}" -B "${BUILD}" -DCMAKE_BUILD_TYPE=RelWithDebInfo
cmake --build "${BUILD}"

rm -f "${DATA}"

RUN=( perf record -g -F 997 -o "${DATA}" -- "${BUILD}/workload_demo" "${N}" "${KIND}"
      "${REPEAT}" )

if command -v taskset >/dev/null 2>&1; then
  taskset -c "${CPU}" "${RUN[@]}"
else
  "${RUN[@]}"
fi

echo
echo "=== perf report (top lines) ==="
perf report -i "${DATA}" --stdio --no-children | head -40

echo
echo "Full TUI: perf report -i ${DATA}"
echo "FlameGraph: ./scripts/run_flamegraph.sh"
