#pragma once

#include <string>
#include <vector>

namespace perf7 {

// 生成约 line_count 行、每行含多个 token 的合成语料（可重复）
std::vector<std::string> MakeCorpus(std::size_t line_count, const std::string& prefix);

}  // namespace perf7
