#include "mini_grep.hpp"

#include <string_view>

namespace perf7 {

namespace {

bool ContainsSubstring(const std::string& hay, const std::string& needle) {
  if (needle.empty() || hay.size() < needle.size()) return false;
  for (std::size_t i = 0; i + needle.size() <= hay.size(); ++i) {
    bool ok = true;
    for (std::size_t j = 0; j < needle.size(); ++j) {
      if (hay[i + j] != needle[j]) {
        ok = false;
        break;
      }
    }
    if (ok) return true;
  }
  return false;
}

}  // namespace

std::size_t CountLinesSlow(const std::vector<std::string>& lines,
                           const std::string& needle) {
  std::size_t count = 0;
  for (const std::string& line : lines) {
    std::string copy = line;
    if (ContainsSubstring(copy, needle)) ++count;
  }
  return count;
}

std::size_t CountLinesFast(const std::vector<std::string>& lines,
                           const std::string& needle) {
  std::size_t count = 0;
  for (const std::string& line : lines) {
    std::string_view sv(line);
    if (sv.find(needle) != std::string_view::npos) ++count;
  }
  return count;
}

}  // namespace perf7
