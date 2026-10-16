#pragma once

#include <cstddef>
#include <string>
#include <vector>

namespace perf7 {

// 故意低效：每行拷贝 string、逐字符比较子串
std::size_t CountLinesSlow(const std::vector<std::string>& lines, const std::string& needle);

// 改进：string_view + find，无逐行堆分配
std::size_t CountLinesFast(const std::vector<std::string>& lines, const std::string& needle);

}  // namespace perf7
