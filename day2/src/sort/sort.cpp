#include "sort.hpp"

#include <algorithm>
#include <random>

namespace perf2 {

std::vector<int> MakeInput(std::size_t n, InputPattern pattern, std::uint32_t seed)
{
  std::vector<int> data(n);
  std::mt19937 rng(seed); // 伪随机数生成器
  std::uniform_int_distribution<int> dist(0, static_cast<int>(n * 10)); // 均匀分布

  switch (pattern) // 根据输入模式生成不同的输入
  {
    case InputPattern::Random:
      for (auto& value : data) // 随机生成输入
      {
        value = dist(rng);
      }
      break;
    case InputPattern::Sorted:
      for (std::size_t i = 0; i < n; ++i) // 有序输入
      {
        data[i] = static_cast<int>(i);
      }
      break;
    case InputPattern::Reversed:
      for (std::size_t i = 0; i < n; ++i) // 逆序输入
      {
        data[i] = static_cast<int>(n - i);
      }
      break;
    case InputPattern::NearlySorted:
      for (std::size_t i = 0; i < n; ++i) // 近乎有序输入
      {
        data[i] = static_cast<int>(i);
      }
      for (std::size_t i = 0; i < n / 100 + 1; ++i) // 随机交换一些元素
      {
        const auto a = static_cast<std::size_t>(dist(rng) % n);
        const auto b = static_cast<std::size_t>(dist(rng) % n);
        std::swap(data[a], data[b]);
      }
      break;
  }
  return data;
}

void BubbleSort(std::vector<int>& data)
{
  const std::size_t n = data.size();
  for (std::size_t i = 0; i < n; ++i)
  {
    for (std::size_t j = 1; j + i < n; ++j)
    {
      if (data[j - 1] > data[j]) {
        std::swap(data[j - 1], data[j]);
      }
    }
  }
}

void StdSort(std::vector<int>& data) { std::sort(data.begin(), data.end()); }

}  // namespace perf2
