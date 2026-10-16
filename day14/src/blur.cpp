#include "blur.hpp"

#include <algorithm>

namespace perf14 {

namespace {

inline std::size_t Idx(std::size_t x, std::size_t y, std::size_t width) {
  return y * width + x;
}

inline std::uint8_t At(const Image& img, std::size_t width, std::size_t height, int x, int y) {
  x = std::max(0, std::min(x, static_cast<int>(width) - 1));
  y = std::max(0, std::min(y, static_cast<int>(height) - 1));
  return img[Idx(static_cast<std::size_t>(x), static_cast<std::size_t>(y), width)];
}

}  // namespace

Image MakeGradient(std::size_t width, std::size_t height) {
  Image img(width * height);
  for (std::size_t y = 0; y < height; ++y)
    for (std::size_t x = 0; x < width; ++x)
      img[Idx(x, y, width)] = static_cast<std::uint8_t>((x + y) % 256);
  return img;
}

void BoxBlur3x3Naive(const Image& src, Image& dst, std::size_t width, std::size_t height) {
  dst.resize(width * height);
  for (std::size_t y = 0; y < height; ++y) {
    for (std::size_t x = 0; x < width; ++x) {
      int sum = 0;
      for (int dy = -1; dy <= 1; ++dy)
        for (int dx = -1; dx <= 1; ++dx)
          sum += At(src, width, height, static_cast<int>(x) + dx, static_cast<int>(y) + dy);
      dst[Idx(x, y, width)] = static_cast<std::uint8_t>(sum / 9);
    }
  }
}

void BoxBlurSeparable(const Image& src, Image& dst, std::size_t width, std::size_t height) {
  Image tmp(width * height);
  for (std::size_t y = 0; y < height; ++y) {
    for (std::size_t x = 0; x < width; ++x) {
      int sum = At(src, width, height, static_cast<int>(x) - 1, static_cast<int>(y)) +
                At(src, width, height, static_cast<int>(x), static_cast<int>(y)) +
                At(src, width, height, static_cast<int>(x) + 1, static_cast<int>(y));
      tmp[Idx(x, y, width)] = static_cast<std::uint8_t>(sum / 3);
    }
  }
  for (std::size_t y = 0; y < height; ++y) {
    for (std::size_t x = 0; x < width; ++x) {
      int sum = At(tmp, width, height, static_cast<int>(x), static_cast<int>(y) - 1) +
                At(tmp, width, height, static_cast<int>(x), static_cast<int>(y)) +
                At(tmp, width, height, static_cast<int>(x), static_cast<int>(y) + 1);
      dst[Idx(x, y, width)] = static_cast<std::uint8_t>(sum / 3);
    }
  }
}

void BoxBlurSeparableTiled(const Image& src, Image& dst, std::size_t width, std::size_t height,
                           std::size_t tile) {
  Image tmp(width * height);
  for (std::size_t y0 = 0; y0 < height; y0 += tile) {
    const std::size_t y_end = std::min(y0 + tile, height);
    for (std::size_t x0 = 0; x0 < width; x0 += tile) {
      const std::size_t x_end = std::min(x0 + tile, width);
      for (std::size_t y = y0; y < y_end; ++y) {
        for (std::size_t x = x0; x < x_end; ++x) {
          int sum = At(src, width, height, static_cast<int>(x) - 1, static_cast<int>(y)) +
                    At(src, width, height, static_cast<int>(x), static_cast<int>(y)) +
                    At(src, width, height, static_cast<int>(x) + 1, static_cast<int>(y));
          tmp[Idx(x, y, width)] = static_cast<std::uint8_t>(sum / 3);
        }
      }
    }
  }
  for (std::size_t y0 = 0; y0 < height; y0 += tile) {
    const std::size_t y_end = std::min(y0 + tile, height);
    for (std::size_t x0 = 0; x0 < width; x0 += tile) {
      const std::size_t x_end = std::min(x0 + tile, width);
      for (std::size_t y = y0; y < y_end; ++y) {
        for (std::size_t x = x0; x < x_end; ++x) {
          int sum = At(tmp, width, height, static_cast<int>(x), static_cast<int>(y) - 1) +
                    At(tmp, width, height, static_cast<int>(x), static_cast<int>(y)) +
                    At(tmp, width, height, static_cast<int>(x), static_cast<int>(y) + 1);
          dst[Idx(x, y, width)] = static_cast<std::uint8_t>(sum / 3);
        }
      }
    }
  }
}

}  // namespace perf14
