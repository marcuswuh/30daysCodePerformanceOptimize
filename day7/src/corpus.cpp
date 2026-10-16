#include "corpus.hpp"

#include <sstream>

namespace perf7 {

std::vector<std::string> MakeCorpus(std::size_t line_count, const std::string& prefix) {
  std::vector<std::string> lines;
  lines.reserve(line_count);
  for (std::size_t i = 0; i < line_count; ++i) {
    std::ostringstream os;
    os << prefix << (i % 997) << " token=" << (i * 17 % 10000) << " status="
       << ((i % 10 == 0) ? "ERROR" : "OK");
    lines.push_back(os.str());
  }
  return lines;
}

}  // namespace perf7
