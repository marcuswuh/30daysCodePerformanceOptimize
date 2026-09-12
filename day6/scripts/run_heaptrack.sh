#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BUILD="${ROOT}/build"
N="${1:-100000}"
MODE="${2:-bad}"

if ! command -v heaptrack >/dev/null 2>&1; then
  echo "heaptrack not found. Install: sudo apt install heaptrack heaptrack-gui"
  exit 1
fi

cmake -S "${ROOT}" -B "${BUILD}" -DCMAKE_BUILD_TYPE=RelWithDebInfo
cmake --build "${BUILD}"

OUT="${ROOT}/heaptrack.mem_demo.${MODE}.gz"
heaptrack -o "${OUT}" "${BUILD}/mem_demo" "${N}" "${MODE}"
echo "Saved: ${OUT}"
echo "GUI: heaptrack_gui ${OUT}"
