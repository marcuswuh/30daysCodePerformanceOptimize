#pragma once

#include <atomic>
#include <cstddef>
#include <iostream>

namespace perf6 {

struct AllocCounter {
  static std::atomic<std::size_t>& News();
  static std::atomic<std::size_t>& Deletes();
  static std::atomic<std::size_t>& Bytes();

  static void Reset();
  static void PrintSummary(const char* label);
};

}  // namespace perf6
