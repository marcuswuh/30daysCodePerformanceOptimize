#include "alloc_counter.hpp"

#include <cstdlib>
#include <new>

namespace perf6 {

static std::atomic<std::size_t> g_news{0};
static std::atomic<std::size_t> g_deletes{0};
static std::atomic<std::size_t> g_bytes{0};

std::atomic<std::size_t>& AllocCounter::News() { return g_news; }
std::atomic<std::size_t>& AllocCounter::Deletes() { return g_deletes; }
std::atomic<std::size_t>& AllocCounter::Bytes() { return g_bytes; }

void AllocCounter::Reset() {
  g_news.store(0, std::memory_order_relaxed);
  g_deletes.store(0, std::memory_order_relaxed);
  g_bytes.store(0, std::memory_order_relaxed);
}

void AllocCounter::PrintSummary(const char* label) {
  std::cout << label << " new=" << g_news.load(std::memory_order_relaxed)
            << " delete=" << g_deletes.load(std::memory_order_relaxed)
            << " bytes=" << g_bytes.load(std::memory_order_relaxed) << '\n';
}

}  // namespace perf6

void* operator new(std::size_t size) {
  void* p = std::malloc(size);
  if (p == nullptr) {
    throw std::bad_alloc{};
  }
  perf6::AllocCounter::News().fetch_add(1, std::memory_order_relaxed);
  perf6::AllocCounter::Bytes().fetch_add(size, std::memory_order_relaxed);
  return p;
}

void operator delete(void* p) noexcept {
  if (p != nullptr) {
    perf6::AllocCounter::Deletes().fetch_add(1, std::memory_order_relaxed);
  }
  std::free(p);
}

void operator delete(void* p, std::size_t) noexcept { ::operator delete(p); }
