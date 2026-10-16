#include "string_ops.hpp"

#include <benchmark/benchmark.h>

namespace {

void BM_CountBad(benchmark::State& state) {
  const auto n = static_cast<std::size_t>(state.range(0));
  const auto lines = perf13::MakeLines(n, 64);
  const std::string prefix = "prefix_1";
  for (auto _ : state) {
    const auto c = perf13::CountPrefixBad(lines, prefix);
    benchmark::DoNotOptimize(c);
  }
  state.SetItemsProcessed(static_cast<int64_t>(state.iterations()) * static_cast<int64_t>(n));
}
BENCHMARK(BM_CountBad)->Arg(200000);

void BM_CountGood(benchmark::State& state) {
  const auto n = static_cast<std::size_t>(state.range(0));
  const auto lines = perf13::MakeLines(n, 64);
  const std::string_view prefix = "prefix_1";
  for (auto _ : state) {
    const auto c = perf13::CountPrefixGood(lines, prefix);
    benchmark::DoNotOptimize(c);
  }
  state.SetItemsProcessed(static_cast<int64_t>(state.iterations()) * static_cast<int64_t>(n));
}
BENCHMARK(BM_CountGood)->Arg(200000);

void BM_PathBad(benchmark::State& state) {
  const std::vector<std::string_view> parts = {"usr", "local", "bin", "app", "data"};
  for (auto _ : state) {
    const auto p = perf13::BuildPathBad(parts);
    benchmark::DoNotOptimize(p);
  }
}
BENCHMARK(BM_PathBad);

void BM_PathGood(benchmark::State& state) {
  const std::vector<std::string_view> parts = {"usr", "local", "bin", "app", "data"};
  for (auto _ : state) {
    const auto p = perf13::BuildPathGood(parts);
    benchmark::DoNotOptimize(p);
  }
}
BENCHMARK(BM_PathGood);

}  // namespace

BENCHMARK_MAIN();
