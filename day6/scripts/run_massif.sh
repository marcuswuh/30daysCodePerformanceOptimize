#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BUILD="${ROOT}/build"
N="${1:-50000}"
MODE="${2:-bad}"

if ! command -v valgrind >/dev/null 2>&1; then
  echo "valgrind not found. Install: sudo apt install valgrind"
  exit 1
fi

cmake -S "${ROOT}" -B "${BUILD}" -DCMAKE_BUILD_TYPE=RelWithDebInfo
cmake --build "${BUILD}"

OUT="${ROOT}/massif.out.${MODE}"
valgrind --tool=massif --massif-out-file="${OUT}" --stacks=yes \
  "${BUILD}/mem_demo" "${N}" "${MODE}"
echo "Saved: ${OUT}"
echo "Report: ms_print ${OUT} | less"
