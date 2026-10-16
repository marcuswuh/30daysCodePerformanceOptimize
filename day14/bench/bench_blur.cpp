#include "blur.hpp"

#include <benchmark/benchmark.h>

namespace {

constexpr std::size_t kW = 2048;
constexpr std::size_t kH = 2048;

void BM_Naive(benchmark::State& state) {
  const auto src = perf14::MakeGradient(kW, kH);
  perf14::Image dst;
  for (auto _ : state) {
    perf14::BoxBlur3x3Naive(src, dst, kW, kH);
    benchmark::DoNotOptimize(dst.data());
  }
  state.SetBytesProcessed(static_cast<int64_t>(state.iterations()) * kW * kH);
}
BENCHMARK(BM_Naive);

void BM_Separable(benchmark::State& state) {
  const auto src = perf14::MakeGradient(kW, kH);
  perf14::Image dst(kW * kH);
  for (auto _ : state) {
    perf14::BoxBlurSeparable(src, dst, kW, kH);
    benchmark::DoNotOptimize(dst.data());
  }
  state.SetBytesProcessed(static_cast<int64_t>(state.iterations()) * kW * kH);
}
BENCHMARK(BM_Separable);

void BM_SeparableTiled(benchmark::State& state) {
  const auto src = perf14::MakeGradient(kW, kH);
  perf14::Image dst(kW * kH);
  for (auto _ : state) {
    perf14::BoxBlurSeparableTiled(src, dst, kW, kH, 64);
    benchmark::DoNotOptimize(dst.data());
  }
  state.SetBytesProcessed(static_cast<int64_t>(state.iterations()) * kW * kH);
}
BENCHMARK(BM_SeparableTiled);

}  // namespace

BENCHMARK_MAIN();
