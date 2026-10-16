#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BUILD="${ROOT}/build"
N="${1:-30000}"
cmake -S "${ROOT}" -B "${BUILD}" -DCMAKE_BUILD_TYPE=RelWithDebInfo
cmake --build "${BUILD}"
if ! command -v heaptrack >/dev/null 2>&1; then
  echo "Install heaptrack or use ./build/alloc_demo"
  exit 1
fi
heaptrack -o "${ROOT}/heaptrack.heap.gz" "${BUILD}/alloc_demo" "${N}"
heaptrack -o "${ROOT}/heaptrack.pool.gz" "${BUILD}/bench_alloc" --benchmark_filter=PoolList
