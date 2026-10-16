#include "layout.hpp"

#include <benchmark/benchmark.h>

#include <cstdint>

namespace {

void BM_IntegrateAoS(benchmark::State& state) {
  const auto n = static_cast<std::size_t>(state.range(0));
  auto particles = perf9::MakeAoS(n);
  for (auto _ : state) {
    perf9::IntegrateAoS(particles, 0.001f);
    benchmark::ClobberMemory();
  }
  state.SetItemsProcessed(static_cast<int64_t>(state.iterations()) * static_cast<int64_t>(n));
}
BENCHMARK(BM_IntegrateAoS)->RangeMultiplier(10)->Range(10000, 1000000);

void BM_IntegrateSoA(benchmark::State& state) {
  const auto n = static_cast<std::size_t>(state.range(0));
  auto particles = perf9::MakeSoA(n);
  for (auto _ : state) {
    perf9::IntegrateSoA(particles, 0.001f);
    benchmark::ClobberMemory();
  }
  state.SetItemsProcessed(static_cast<int64_t>(state.iterations()) * static_cast<int64_t>(n));
}
BENCHMARK(BM_IntegrateSoA)->RangeMultiplier(10)->Range(10000, 1000000);

void BM_SumHotColdBad(benchmark::State& state) {
  const auto n = static_cast<std::size_t>(state.range(0));
  const auto entities = perf9::MakeEntitiesBad(n);
  for (auto _ : state) {
    const float s = perf9::SumPositionsBad(entities);
    benchmark::DoNotOptimize(s);
  }
  state.SetItemsProcessed(static_cast<int64_t>(state.iterations()) * static_cast<int64_t>(n));
}
BENCHMARK(BM_SumHotColdBad)->Arg(500000);

void BM_SumHotColdGood(benchmark::State& state) {
  const auto n = static_cast<std::size_t>(state.range(0));
  const auto entities = perf9::MakeEntitiesGood(n);
  for (auto _ : state) {
    const float s = perf9::SumPositionsGood(entities);
    benchmark::DoNotOptimize(s);
  }
  state.SetItemsProcessed(static_cast<int64_t>(state.iterations()) * static_cast<int64_t>(n));
}
BENCHMARK(BM_SumHotColdGood)->Arg(500000);

}  // namespace

BENCHMARK_MAIN();
