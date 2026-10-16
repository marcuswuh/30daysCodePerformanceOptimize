#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

namespace perf11 {

enum class DataPattern { kRandom, kSorted, kMostlyPass };

std::vector<int> MakeData(std::size_t n, DataPattern pattern);

std::uint64_t SumIfBranch(const std::vector<int>& data, int threshold);
std::uint64_t SumIfBranchless(const std::vector<int>& data, int threshold);

// 小分支：查表代替 switch
int ClassifySwitch(int x);
int ClassifyLut(int x);

std::size_t CountClassifySwitch(const std::vector<int>& data);
std::size_t CountClassifyLut(const std::vector<int>& data);

}  // namespace perf11
