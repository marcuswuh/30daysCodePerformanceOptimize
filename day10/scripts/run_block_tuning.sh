#!/usr/bin/env bash
set -euo pipefail
# 选做：在源码中改 block_size 或增加 BM_Blocked* 后对比；此处仅跑默认基准
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cmake -S "${ROOT}" -B "${ROOT}/build" -DCMAKE_BUILD_TYPE=RelWithDebInfo
cmake --build "${ROOT}/build"
"${ROOT}/build/bench_matmul" --benchmark_repetitions=5 --benchmark_report_aggregates_only=true
