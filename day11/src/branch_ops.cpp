#include "branch_ops.hpp"

#include <algorithm>
#include <random>

namespace perf11 {

std::vector<int> MakeData(std::size_t n, DataPattern pattern) {
  std::vector<int> data(n);
  std::mt19937 rng(123);
  switch (pattern) {
    case DataPattern::kRandom: {
      std::uniform_int_distribution<int> dist(0, 100);
      for (int& v : data) v = dist(rng);
      break;
    }
    case DataPattern::kSorted: {
      std::uniform_int_distribution<int> dist(0, 100);
      for (int& v : data) v = dist(rng);
      std::sort(data.begin(), data.end());
      break;
    }
    case DataPattern::kMostlyPass: {
      for (std::size_t i = 0; i < n; ++i) data[i] = (i % 10 == 0) ? 10 : 0;
      break;
    }
  }
  return data;
}

std::uint64_t SumIfBranch(const std::vector<int>& data, int threshold) {
  std::uint64_t sum = 0;
  for (int v : data) {
    if (v > threshold) sum += static_cast<std::uint64_t>(v);
  }
  return sum;
}

std::uint64_t SumIfBranchless(const std::vector<int>& data, int threshold) {
  std::uint64_t sum = 0;
  for (int v : data) {
    sum += static_cast<std::uint64_t>(v) * static_cast<std::uint64_t>(v > threshold);
  }
  return sum;
}

int ClassifySwitch(int x) {
  switch (x % 8) {
    case 0: return 1;
    case 1: return 2;
    case 2: return 3;
    case 3: return 4;
    case 4: return 5;
    case 5: return 6;
    case 6: return 7;
    default: return 8;
  }
}

int ClassifyLut(int x) {
  static constexpr int kTable[8] = {1, 2, 3, 4, 5, 6, 7, 8};
  return kTable[static_cast<unsigned>(x % 8)];
}

std::size_t CountClassifySwitch(const std::vector<int>& data) {
  std::size_t s = 0;
  for (int v : data) s += static_cast<std::size_t>(ClassifySwitch(v));
  return s;
}

std::size_t CountClassifyLut(const std::vector<int>& data) {
  std::size_t s = 0;
  for (int v : data) s += static_cast<std::size_t>(ClassifyLut(v));
  return s;
}

}  // namespace perf11
