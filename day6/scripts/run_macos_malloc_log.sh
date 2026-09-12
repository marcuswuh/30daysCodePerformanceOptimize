#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BUILD="${ROOT}/build"
N="${1:-50000}"
MODE="${2:-bad}"

cmake -S "${ROOT}" -B "${BUILD}" -DCMAKE_BUILD_TYPE=RelWithDebInfo
cmake --build "${BUILD}"

# 仅 macOS：用 malloc 日志 + leaks 做轻量替代（无 heaptrack 时的课程路径）
export MallocStackLogging=1
export MallocScribble=1

echo "Running mem_demo (watch allocation pressure in Instruments if needed)..."
/usr/bin/time -l "${BUILD}/mem_demo" "${N}" "${MODE}"

echo
echo "Custom counter demo (same machine, no extra tools):"
"${BUILD}/alloc_trace_demo" "${N}" "${MODE}"

echo
echo "Tip: open Instruments -> Allocations, attach/run mem_demo for call stacks."
