#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

namespace perf3 {

enum class WorkloadKind { kRowMajor, kColMajor, kBranchy };

// 行主序矩阵求和：cache 友好
float SumRowMajor(const std::vector<float>& m, std::size_t n);

// 列主序访问：cache miss 高
float SumColMajor(const std::vector<float>& m, std::size_t n);

// 数据相关分支：branch-misses 偏高
std::uint64_t CountBranchy(const std::vector<int>& data, int threshold);

void RunWorkload(WorkloadKind kind, std::size_t n, int repeat);

}  // namespace perf3
