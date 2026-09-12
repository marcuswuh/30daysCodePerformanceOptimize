#include <benchmark/benchmark.h>

#include "sort.hpp"

#include <cstdint>
#include <string>
#include <vector>

namespace {

perf2::InputPattern PatternFromName(const std::string& name)
{
  if (name.find("nearly_sorted") != std::string::npos)
  {
    return perf2::InputPattern::NearlySorted;
  }
  if (name.find("reversed") != std::string::npos)
  {
    return perf2::InputPattern::Reversed;
  }
  if (name.find("sorted") != std::string::npos)
  {
    return perf2::InputPattern::Sorted;
  }
  return perf2::InputPattern::Random;
}

void BM_StdSort(benchmark::State& state) {
  const auto n = static_cast<std::size_t>(state.range(0));
  const auto pattern = PatternFromName(state.name());

  for (auto _ : state) {
    state.PauseTiming();
    auto data = perf2::MakeInput(n, pattern);
    state.ResumeTiming();

    perf2::StdSort(data);
    benchmark::DoNotOptimize(data.data());
  }

  state.SetComplexityN(static_cast<int64_t>(n));
  state.SetItemsProcessed(static_cast<int64_t>(state.iterations()) *
                          static_cast<int64_t>(n));
}
BENCHMARK(BM_StdSort)
    ->Name("StdSort/random")
    ->RangeMultiplier(10)
    ->Range(1'000, 100'000)
    ->Complexity(benchmark::oNLogN);

BENCHMARK(BM_StdSort)
    ->Name("StdSort/sorted")
    ->RangeMultiplier(10)
    ->Range(1'000, 100'000)
    ->Complexity(benchmark::oNLogN);

BENCHMARK(BM_StdSort)
    ->Name("StdSort/reversed")
    ->RangeMultiplier(10)
    ->Range(1'000, 100'000)
    ->Complexity(benchmark::oNLogN);

BENCHMARK(BM_StdSort)
    ->Name("StdSort/nearly_sorted")
    ->RangeMultiplier(10)
    ->Range(1'000, 100'000)
    ->Complexity(benchmark::oNLogN);

void BM_BubbleSort(benchmark::State& state) {
  const auto n = static_cast<std::size_t>(state.range(0));

  for (auto _ : state) {
    state.PauseTiming();
    auto data = perf2::MakeInput(n, perf2::InputPattern::Random);
    state.ResumeTiming();

    perf2::BubbleSort(data);
    benchmark::DoNotOptimize(data.data());
  }

  state.SetComplexityN(static_cast<int64_t>(n));
  state.SetItemsProcessed(static_cast<int64_t>(state.iterations()) *
                          static_cast<int64_t>(n));
}
BENCHMARK(BM_BubbleSort)
    ->Name("BubbleSort")
    ->RangeMultiplier(10)
    ->Range(100, 1'000)
    ->Complexity(benchmark::oN);

}  // namespace

BENCHMARK_MAIN();
