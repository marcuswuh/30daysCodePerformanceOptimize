#include "blur.hpp"

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <string>

int main(int argc, char** argv) {
  const std::size_t w = (argc > 1) ? static_cast<std::size_t>(std::stoull(argv[1])) : 2048;
  const std::size_t h = (argc > 2) ? static_cast<std::size_t>(std::stoull(argv[2])) : 2048;
  const std::string mode = (argc > 3) ? argv[3] : "all";

  const auto src = perf14::MakeGradient(w, h);
  perf14::Image dst;

  const auto run = [&](const char* tag, auto fn) {
    const auto t0 = std::chrono::steady_clock::now();
    fn();
    const auto t1 = std::chrono::steady_clock::now();
    const auto ms =
        std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
    std::cout << tag << " ms=" << ms << " checksum=" << static_cast<int>(dst[0]) << '\n';
  };

  if (mode == "naive" || mode == "all") {
    run("naive     ", [&] { perf14::BoxBlur3x3Naive(src, dst, w, h); });
  }
  if (mode == "sep" || mode == "all") {
    dst.assign(w * h, 0);
    run("separable ", [&] { perf14::BoxBlurSeparable(src, dst, w, h); });
  }
  if (mode == "tiled" || mode == "all") {
    dst.assign(w * h, 0);
    run("tiled     ", [&] { perf14::BoxBlurSeparableTiled(src, dst, w, h, 64); });
  }
  return 0;
}
