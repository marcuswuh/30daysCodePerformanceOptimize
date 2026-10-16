#include "workload.hpp"

#include <chrono>
#include <iostream>
#include <random>

namespace perf3 {

float SumRowMajor(const std::vector<float>& m, std::size_t n) {
  float sum = 0.0f;
  for (std::size_t i = 0; i < n; ++i)
    for (std::size_t j = 0; j < n; ++j) sum += m[i * n + j];
  return sum;
}

float SumColMajor(const std::vector<float>& m, std::size_t n) {
  float sum = 0.0f;
  for (std::size_t j = 0; j < n; ++j)
    for (std::size_t i = 0; i < n; ++i) sum += m[i * n + j];
  return sum;
}

std::uint64_t CountBranchy(const std::vector<int>& data, int threshold) {
  std::uint64_t hits = 0;
  for (int v : data) {
    if (v > threshold) {
      hits += static_cast<std::uint64_t>(v);
    }
  }
  return hits;
}

void RunWorkload(WorkloadKind kind, std::size_t n, int repeat) {
  std::vector<float> matrix(n * n, 1.0f);
  std::vector<int> branch_data(n * n);
  std::mt19937 rng(42);
  std::uniform_int_distribution<int> dist(0, 100);
  for (int& v : branch_data) v = dist(rng);

  const int threshold = 50;
  volatile float sink_f = 0.0f;
  volatile std::uint64_t sink_u = 0;

  const auto t0 = std::chrono::steady_clock::now();
  for (int r = 0; r < repeat; ++r) {
    switch (kind) {
      case WorkloadKind::kRowMajor:
        sink_f += SumRowMajor(matrix, n);
        break;
      case WorkloadKind::kColMajor:
        sink_f += SumColMajor(matrix, n);
        break;
      case WorkloadKind::kBranchy:
        sink_u += CountBranchy(branch_data, threshold);
        break;
    }
  }
  const auto t1 = std::chrono::steady_clock::now();
  const auto ms =
      std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();

  std::cout << "kind=" << static_cast<int>(kind) << " n=" << n << " repeat=" << repeat
            << " sink_f=" << sink_f << " sink_u=" << sink_u << " time_ms=" << ms << '\n';
}

}  // namespace perf3
