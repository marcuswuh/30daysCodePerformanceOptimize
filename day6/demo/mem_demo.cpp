#include "match.hpp"

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <string>

int main(int argc, char** argv) {
  const std::size_t n = (argc > 1) ? static_cast<std::size_t>(std::stoull(argv[1])) : 200'000;
  const std::string mode = (argc > 2) ? argv[2] : "both";

  const auto items = perf6::MakeItems(n, "user_");
  const std::string prefix = "user_1";

  const auto run = [&](const char* tag, auto fn) {
    const auto t0 = std::chrono::steady_clock::now();
    const std::size_t result = fn();
    const auto t1 = std::chrono::steady_clock::now();
    const auto ms =
        std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
    std::cout << tag << " matched=" << result << " time_ms=" << ms << '\n';
  };

  if (mode == "bad" || mode == "both") {
    run("bad ", [&] { return perf6::CountMatchesBad(items, prefix); });
  }
  if (mode == "good" || mode == "both") {
    run("good", [&] { return perf6::CountMatchesGood(items, prefix); });
  }

  return 0;
}
