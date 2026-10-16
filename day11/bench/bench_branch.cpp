#include "branch_ops.hpp"

#include <benchmark/benchmark.h>

#include <cstdint>
#include <string>

namespace {

perf11::DataPattern PatternFromName(const std::string& name) {
  if (name.find("sorted") != std::string::npos) return perf11::DataPattern::kSorted;
  if (name.find("mostly") != std::string::npos) return perf11::DataPattern::kMostlyPass;
  return perf11::DataPattern::kRandom;
}

void BM_SumBranch(benchmark::State& state) {
  const auto n = static_cast<std::size_t>(state.range(0));
  const auto pattern = PatternFromName(state.name());
  const auto data = perf11::MakeData(n, pattern);
  constexpr int kThreshold = 50;
  for (auto _ : state) {
    const auto s = perf11::SumIfBranch(data, kThreshold);
    benchmark::DoNotOptimize(s);
  }
  state.SetItemsProcessed(static_cast<int64_t>(state.iterations()) * static_cast<int64_t>(n));
}
BENCHMARK(BM_SumBranch)->Name("SumBranch/random")->Arg(1 << 20);
BENCHMARK(BM_SumBranch)->Name("SumBranch/mostly_pass")->Arg(1 << 20);

void BM_SumBranchless(benchmark::State& state) {
  const auto n = static_cast<std::size_t>(state.range(0));
  const auto pattern = PatternFromName(state.name());
  const auto data = perf11::MakeData(n, pattern);
  constexpr int kThreshold = 50;
  for (auto _ : state) {
    const auto s = perf11::SumIfBranchless(data, kThreshold);
    benchmark::DoNotOptimize(s);
  }
  state.SetItemsProcessed(static_cast<int64_t>(state.iterations()) * static_cast<int64_t>(n));
}
BENCHMARK(BM_SumBranchless)->Name("SumBranchless/random")->Arg(1 << 20);
BENCHMARK(BM_SumBranchless)->Name("SumBranchless/mostly_pass")->Arg(1 << 20);

void BM_ClassifySwitch(benchmark::State& state) {
  const auto n = static_cast<std::size_t>(state.range(0));
  const auto data = perf11::MakeData(n, perf11::DataPattern::kRandom);
  for (auto _ : state) {
    const auto s = perf11::CountClassifySwitch(data);
    benchmark::DoNotOptimize(s);
  }
  state.SetItemsProcessed(static_cast<int64_t>(state.iterations()) * static_cast<int64_t>(n));
}
BENCHMARK(BM_ClassifySwitch)->Arg(500000);

void BM_ClassifyLut(benchmark::State& state) {
  const auto n = static_cast<std::size_t>(state.range(0));
  const auto data = perf11::MakeData(n, perf11::DataPattern::kRandom);
  for (auto _ : state) {
    const auto s = perf11::CountClassifyLut(data);
    benchmark::DoNotOptimize(s);
  }
  state.SetItemsProcessed(static_cast<int64_t>(state.iterations()) * static_cast<int64_t>(n));
}
BENCHMARK(BM_ClassifyLut)->Arg(500000);

}  // namespace

BENCHMARK_MAIN();
