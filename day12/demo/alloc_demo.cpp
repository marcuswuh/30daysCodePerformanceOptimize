#include "alloc_demo.hpp"

#include <chrono>
#include <cstdlib>
#include <iostream>

int main(int argc, char** argv) {
  const std::size_t n = (argc > 1) ? static_cast<std::size_t>(std::stoull(argv[1])) : 50000;
  perf12::NodePool pool;

  const auto bench = [](const char* tag, auto fn) {
    const auto t0 = std::chrono::steady_clock::now();
    const std::size_t s = fn();
    const auto t1 = std::chrono::steady_clock::now();
    const auto ms =
        std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
    std::cout << tag << " sum=" << s << " ms=" << ms << '\n';
  };

  bench("heap", [&] { return perf12::BuildListHeap(n); });
  bench("pool", [&] { return perf12::BuildListPool(pool, n); });
  bench("default_vec", [&] { return perf12::SumDefaultVectors(2000, 128); });
  bench("pmr_vec    ", [&] { return perf12::SumPmrVectors(2000, 128); });
  return 0;
}
