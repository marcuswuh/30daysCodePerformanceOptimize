#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
DATA="${ROOT}/perf.data"
OUT="${ROOT}/flamegraph.svg"
FG="${FLAMEGRAPH_HOME:-${HOME}/tools/FlameGraph}"

if [[ ! -f "${DATA}" ]]; then
  echo "Missing ${DATA}. Run ./scripts/run_perf_record.sh first."
  exit 1
fi

if [[ ! -x "${FG}/stackcollapse-perf.pl" ]]; then
  echo "FlameGraph not found at ${FG}"
  echo "Clone: git clone https://github.com/brendangregg/FlameGraph ${FG}"
  exit 1
fi

perf script -i "${DATA}" | "${FG}/stackcollapse-perf.pl" | "${FG}/flamegraph.pl" > "${OUT}"
echo "Wrote ${OUT}"
