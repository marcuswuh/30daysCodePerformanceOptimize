#include "corpus.hpp"
#include "mini_grep.hpp"

#include <benchmark/benchmark.h>

namespace {

void BM_Slow(benchmark::State& state) {
  const std::size_t lines = static_cast<std::size_t>(state.range(0));
  const auto corpus = perf7::MakeCorpus(lines, "log_line_");
  const std::string needle = "ERROR";

  for (auto _ : state) {
    const std::size_t n = perf7::CountLinesSlow(corpus, needle);
    benchmark::DoNotOptimize(n);
  }
  state.SetItemsProcessed(static_cast<int64_t>(state.iterations()) *
                          static_cast<int64_t>(lines));
}
BENCHMARK(BM_Slow)->Arg(100000);

void BM_Fast(benchmark::State& state) {
  const std::size_t lines = static_cast<std::size_t>(state.range(0));
  const auto corpus = perf7::MakeCorpus(lines, "log_line_");
  const std::string needle = "ERROR";

  for (auto _ : state) {
    const std::size_t n = perf7::CountLinesFast(corpus, needle);
    benchmark::DoNotOptimize(n);
  }
  state.SetItemsProcessed(static_cast<int64_t>(state.iterations()) *
                          static_cast<int64_t>(lines));
}
BENCHMARK(BM_Fast)->Arg(100000);

}  // namespace

BENCHMARK_MAIN();
