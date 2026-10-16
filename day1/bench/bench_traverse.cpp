#include <benchmark/benchmark.h>

#include <cstddef>
#include <cstdint>
#include <vector>

namespace {

constexpr std::size_t kN = 1024;

void BM_RowMajor(benchmark::State& state) {
  std::vector<float> m(kN * kN, 1.0f);
  for (auto _ : state) {
    float sum = 0.0f;
    for (std::size_t i = 0; i < kN; ++i)
      for (std::size_t j = 0; j < kN; ++j) sum += m[i * kN + j];
    benchmark::DoNotOptimize(sum);
  }
  state.SetBytesProcessed(static_cast<std::int64_t>(state.iterations()) * kN * kN *
                          sizeof(float));
}
BENCHMARK(BM_RowMajor);

void BM_ColumnMajor(benchmark::State& state) {
  std::vector<float> m(kN * kN, 1.0f);
  for (auto _ : state) {
    float sum = 0.0f;
    for (std::size_t j = 0; j < kN; ++j)
      for (std::size_t i = 0; i < kN; ++i) sum += m[i * kN + j];
    benchmark::DoNotOptimize(sum);
  }
  state.SetBytesProcessed(static_cast<std::int64_t>(state.iterations()) * kN * kN *
                          sizeof(float));
}
BENCHMARK(BM_ColumnMajor);

}  // namespace

BENCHMARK_MAIN();
