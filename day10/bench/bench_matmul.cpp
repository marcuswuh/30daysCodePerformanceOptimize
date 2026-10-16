#include "matmul.hpp"

#include <benchmark/benchmark.h>

namespace {

constexpr std::size_t kN = 256;

void BM_Ijk(benchmark::State& state) {
  const auto a = perf10::MakeMatrix(kN, 0.5f);
  const auto b = perf10::MakeMatrix(kN, 0.5f);
  perf10::Matrix c(kN * kN);
  for (auto _ : state) {
    perf10::MatMulIjk(a, b, c, kN);
    benchmark::DoNotOptimize(c.data());
  }
}
BENCHMARK(BM_Ijk);

void BM_Ikj(benchmark::State& state) {
  const auto a = perf10::MakeMatrix(kN, 0.5f);
  const auto b = perf10::MakeMatrix(kN, 0.5f);
  perf10::Matrix c(kN * kN);
  for (auto _ : state) {
    perf10::MatMulIkj(a, b, c, kN);
    benchmark::DoNotOptimize(c.data());
  }
}
BENCHMARK(BM_Ikj);

void BM_Blocked32(benchmark::State& state) {
  const auto a = perf10::MakeMatrix(kN, 0.5f);
  const auto b = perf10::MakeMatrix(kN, 0.5f);
  perf10::Matrix c(kN * kN);
  for (auto _ : state) {
    perf10::MatMulBlocked(a, b, c, kN, 32);
    benchmark::DoNotOptimize(c.data());
  }
}
BENCHMARK(BM_Blocked32);

void BM_IkjUnroll4(benchmark::State& state) {
  const auto a = perf10::MakeMatrix(kN, 0.5f);
  const auto b = perf10::MakeMatrix(kN, 0.5f);
  perf10::Matrix c(kN * kN);
  for (auto _ : state) {
    perf10::MatMulIkjUnroll4(a, b, c, kN);
    benchmark::DoNotOptimize(c.data());
  }
}
BENCHMARK(BM_IkjUnroll4);

}  // namespace

BENCHMARK_MAIN();
