#include "corpus.hpp"
#include "mini_grep.hpp"

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <string>

int main(int argc, char** argv) {
  const std::size_t lines = (argc > 1) ? static_cast<std::size_t>(std::stoull(argv[1])) : 500'000;
  const std::string mode = (argc > 2) ? argv[2] : "both";
  const std::string needle = (argc > 3) ? argv[3] : "ERROR";

  const auto corpus = perf7::MakeCorpus(lines, "log_line_");

  const auto run = [&](const char* tag, auto fn) {
    const auto t0 = std::chrono::steady_clock::now();
    const std::size_t n = fn();
    const auto t1 = std::chrono::steady_clock::now();
    const auto ms =
        std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
    std::cout << tag << " matches=" << n << " time_ms=" << ms << '\n';
  };

  if (mode == "slow" || mode == "both") {
    run("slow", [&] { return perf7::CountLinesSlow(corpus, needle); });
  }
  if (mode == "fast" || mode == "both") {
    run("fast", [&] { return perf7::CountLinesFast(corpus, needle); });
  }

  return 0;
}
