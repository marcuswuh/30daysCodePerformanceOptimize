#!/usr/bin/env bash
set -euo pipefail

ok=0
fail=0

check() {
  if "$@"; then
    echo "[OK] $*"
    ok=$((ok + 1))
  else
    echo "[FAIL] $*"
    fail=$((fail + 1))
  fi
}

echo "=== Toolchain ==="
check g++ --version
check clang++ --version 2>/dev/null || check clang++-18 --version
check cmake --version
check ninja --version

echo
echo "=== perf (optional on VM) ==="
if perf stat -e cycles,instructions true 2>/dev/null; then
  echo "[OK] perf stat basic events"
  ok=$((ok + 1))
else
  echo "[WARN] perf unavailable or perf_event_paranoid too high (see day1 README 2.3)"
fi

echo
echo "=== heaptrack (Day 6) ==="
if command -v heaptrack >/dev/null 2>&1; then
  heaptrack --version
  ok=$((ok + 1))
else
  echo "[WARN] heaptrack not installed"
fi

echo
echo "Summary: ${ok} checks passed, ${fail} failed"
exit "${fail}"
