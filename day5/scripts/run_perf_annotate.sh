#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BUILD="${ROOT}/build"
KIND="${1:-col}"
N="${2:-1024}"
REPEAT="${3:-16}"
CPU="${TASKSET_CPU:-2}"
DATA="${ROOT}/perf.data"
SYMBOL="${ANNOTATE_SYMBOL:-SumColMajor}"

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
echo "=== perf annotate: ${SYMBOL} ==="
if perf annotate -i "${DATA}" --stdio "${SYMBOL}" 2>/dev/null | head -60; then
  :
else
  echo "(Try: ANNOTATE_SYMBOL=SumRowMajor $0 row 1024 16)"
  perf annotate -i "${DATA}" --stdio | head -40
fi

echo
echo "Also try: objdump -dS ${BUILD}/workload_demo | less"
echo "Online: https://godbolt.org with workload.cpp loops"
