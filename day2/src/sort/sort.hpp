#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

namespace perf2 {

enum class InputPattern { Random, Sorted, Reversed, NearlySorted };

// 生成输入，根据输入模式生成不同的输入
std::vector<int> MakeInput(std::size_t n, InputPattern pattern, std::uint32_t seed = 42);

// 冒泡排序
void BubbleSort(std::vector<int>& data);
// 标准库排序
void StdSort(std::vector<int>& data);

}  // namespace perf2
