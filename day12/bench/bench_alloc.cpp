#include "alloc_demo.hpp"

#include <benchmark/benchmark.h>

namespace {

void BM_HeapList(benchmark::State& state) {
  const auto n = static_cast<std::size_t>(state.range(0));
  for (auto _ : state) {
    const auto s = perf12::BuildListHeap(n);
    benchmark::DoNotOptimize(s);
  }
}
BENCHMARK(BM_HeapList)->Arg(10000);

void BM_PoolList(benchmark::State& state) {
  const auto n = static_cast<std::size_t>(state.range(0));
  perf12::NodePool pool;
  for (auto _ : state) {
    const auto s = perf12::BuildListPool(pool, n);
    benchmark::DoNotOptimize(s);
    pool.Reset();
  }
}
BENCHMARK(BM_PoolList)->Arg(10000);

void BM_DefaultVectors(benchmark::State& state) {
  constexpr std::size_t kCount = 2000;
  constexpr std::size_t kElems = 128;
  for (auto _ : state) {
    const auto s = perf12::SumDefaultVectors(kCount, kElems);
    benchmark::DoNotOptimize(s);
  }
}
BENCHMARK(BM_DefaultVectors);

void BM_PmrVectors(benchmark::State& state) {
  constexpr std::size_t kCount = 2000;
  constexpr std::size_t kElems = 128;
  for (auto _ : state) {
    const auto s = perf12::SumPmrVectors(kCount, kElems);
    benchmark::DoNotOptimize(s);
  }
}
BENCHMARK(BM_PmrVectors);

}  // namespace

BENCHMARK_MAIN();
