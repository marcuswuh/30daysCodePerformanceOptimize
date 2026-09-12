#include "alloc_counter.hpp"
#include "match.hpp"

#include <cstdlib>
#include <iostream>
#include <string>

int main(int argc, char** argv) {
  const std::size_t n = (argc > 1) ? static_cast<std::size_t>(std::stoull(argv[1])) : 50'000;
  const std::string mode = (argc > 2) ? argv[2] : "both";
  const auto items = perf6::MakeItems(n, "user_");
  const std::string prefix = "user_1";

  if (mode == "bad" || mode == "both") {
    perf6::AllocCounter::Reset();
    const std::size_t bad = perf6::CountMatchesBad(items, prefix);
    perf6::AllocCounter::PrintSummary("bad ");
    std::cout << "bad matched=" << bad << '\n';
  }

  if (mode == "good" || mode == "both") {
    perf6::AllocCounter::Reset();
    const std::size_t good = perf6::CountMatchesGood(items, prefix);
    perf6::AllocCounter::PrintSummary("good");
    std::cout << "good matched=" << good << '\n';
  }

  return 0;
}
