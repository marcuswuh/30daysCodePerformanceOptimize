#include "matrix.hpp"

#include <benchmark/benchmark.h>

#include <cstdint>
#include <vector>

namespace {

void BM_RowMajor(benchmark::State& state) {
  const auto n = static_cast<std::size_t>(state.range(0));
  std::vector<float> m(n * n, 1.0f);
  for (auto _ : state) {
    const float s = perf8::SumRowMajor(m, n);
    benchmark::DoNotOptimize(s);
  }
  state.SetBytesProcessed(static_cast<int64_t>(state.iterations()) * n * n * sizeof(float));
}
BENCHMARK(BM_RowMajor)->RangeMultiplier(2)->Range(64, 4096);

void BM_ColMajor(benchmark::State& state) {
  const auto n = static_cast<std::size_t>(state.range(0));
  std::vector<float> m(n * n, 1.0f);
  for (auto _ : state) {
    const float s = perf8::SumColMajor(m, n);
    benchmark::DoNotOptimize(s);
  }
  state.SetBytesProcessed(static_cast<int64_t>(state.iterations()) * n * n * sizeof(float));
}
BENCHMARK(BM_ColMajor)->RangeMultiplier(2)->Range(64, 4096);

}  // namespace

BENCHMARK_MAIN();
