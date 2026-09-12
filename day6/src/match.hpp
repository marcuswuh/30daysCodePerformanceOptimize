#pragma once

#include <cstddef>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace perf6 {

struct Item {
  std::string name;

  explicit Item(std::string n) : name(std::move(n)) {}
};

inline std::vector<Item> MakeItems(std::size_t count, std::string_view prefix) {
  std::vector<Item> items;
  items.reserve(count);
  for (std::size_t i = 0; i < count; ++i) {
    // 故意加长，避免 libc++ SSO 掩盖堆分配（Day6 教学用）
    items.emplace_back(std::string(prefix) + std::to_string(i) + std::string(64, 'x'));
  }
  return items;
}

inline std::size_t CountMatchesBad(const std::vector<Item>& items, std::string prefix) {
  std::size_t matched = 0;
  for (const Item& item : items) {
    // 每轮完整拷贝长字符串（必走堆），再比前缀 → 分配次数 ~ O(n)
    const std::string scratch = item.name;
    if (scratch.compare(0, prefix.size(), prefix) == 0) {
      ++matched;
    }
  }
  return matched;
}

inline std::size_t CountMatchesGood(const std::vector<Item>& items,
                                    std::string_view prefix) {
  std::size_t matched = 0;
  for (const Item& item : items) {
    const std::string_view name = item.name;
    matched += (name.size() >= prefix.size() &&
                name.compare(0, prefix.size(), prefix) == 0);
  }
  return matched;
}

}  // namespace perf6
