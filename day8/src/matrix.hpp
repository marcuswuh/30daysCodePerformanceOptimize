#pragma once

#include <cstddef>
#include <vector>

namespace perf8 {

inline float SumRowMajor(const std::vector<float>& m, std::size_t n) {
  float sum = 0.0f;
  for (std::size_t i = 0; i < n; ++i)
    for (std::size_t j = 0; j < n; ++j) sum += m[i * n + j];
  return sum;
}

inline float SumColMajor(const std::vector<float>& m, std::size_t n) {
  float sum = 0.0f;
  for (std::size_t j = 0; j < n; ++j)
    for (std::size_t i = 0; i < n; ++i) sum += m[i * n + j];
  return sum;
}

// 按 cache line 步进访问（模拟 false sharing 之外的 line 竞争，仅教学对比）
inline float SumStrided(const std::vector<float>& m, std::size_t n, std::size_t stride_lines) {
  constexpr std::size_t kFloatsPerLine = 64 / sizeof(float);  // 假设 64B cache line
  float sum = 0.0f;
  for (std::size_t line = 0; line < n; line += stride_lines) {
    const std::size_t i = line;
    if (i >= n) break;
    for (std::size_t j = 0; j < n; ++j) sum += m[i * n + j];
  }
  return sum;
}

}  // namespace perf8
