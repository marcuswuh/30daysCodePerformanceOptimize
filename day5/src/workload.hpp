#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

namespace perf3 {

enum class WorkloadKind { kRowMajor, kColMajor, kBranchy };

float SumRowMajor(const std::vector<float>& m, std::size_t n);
float SumColMajor(const std::vector<float>& m, std::size_t n);
std::uint64_t CountBranchy(const std::vector<int>& data, int threshold);
void RunWorkload(WorkloadKind kind, std::size_t n, int repeat);

}  // namespace perf3
