#include "workload.hpp"

#include <cstdlib>
#include <iostream>
#include <string>

namespace {

perf3::WorkloadKind ParseKind(const std::string& s) {
  if (s == "row" || s == "row_major") return perf3::WorkloadKind::kRowMajor;
  if (s == "col" || s == "column" || s == "col_major") return perf3::WorkloadKind::kColMajor;
  if (s == "branch" || s == "branchy") return perf3::WorkloadKind::kBranchy;
  std::cerr << "Unknown kind: " << s << " (use row|col|branch)\n";
  std::exit(2);
}

}  // namespace

int main(int argc, char** argv) {
  const std::size_t n = (argc > 1) ? static_cast<std::size_t>(std::stoull(argv[1])) : 1024;
  const std::string kind_str = (argc > 2) ? argv[2] : "col";
  const int repeat = (argc > 3) ? std::stoi(argv[3]) : 16;

  perf3::RunWorkload(ParseKind(kind_str), n, repeat);
  return 0;
}
