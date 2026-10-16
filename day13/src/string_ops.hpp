#pragma once

#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

namespace perf13 {

// 坏：按值 + 循环内拼接
inline std::size_t CountPrefixBad(const std::vector<std::string>& lines,
                                  std::string prefix) {
  std::size_t n = 0;
  for (const std::string& line : lines) {
    if (line.size() >= prefix.size() &&
        line.substr(0, prefix.size()) == prefix) {
      ++n;
    }
  }
  return n;
}

// 好：string_view，无 substr 分配
inline std::size_t CountPrefixGood(const std::vector<std::string>& lines,
                                   std::string_view prefix) {
  std::size_t n = 0;
  for (const std::string& line : lines) {
    const std::string_view sv(line);
    n += (sv.size() >= prefix.size() && sv.compare(0, prefix.size(), prefix) == 0);
  }
  return n;
}

// 坏：反复 operator+ 构造
inline std::string BuildPathBad(const std::vector<std::string_view>& parts) {
  std::string path;
  for (std::string_view p : parts) path = path + std::string(p) + "/";
  return path;
}

// 好：reserve + append
inline std::string BuildPathGood(const std::vector<std::string_view>& parts) {
  std::size_t total = 0;
  for (std::string_view p : parts) total += p.size() + 1;
  std::string path;
  path.reserve(total);
  for (std::string_view p : parts) {
    path.append(p);
    path.push_back('/');
  }
  return path;
}

inline std::vector<std::string> MakeLines(std::size_t n, std::size_t suffix_len) {
  std::vector<std::string> lines;
  lines.reserve(n);
  const std::string suffix(suffix_len, 'x');
  for (std::size_t i = 0; i < n; ++i) {
    lines.push_back("prefix_" + std::to_string(i) + suffix);
  }
  return lines;
}

}  // namespace perf13
