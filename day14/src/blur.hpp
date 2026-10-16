#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

namespace perf14 {

using Image = std::vector<std::uint8_t>;

Image MakeGradient(std::size_t width, std::size_t height);

// 3x3 盒式模糊，边界 clamp；逐像素重读邻域
void BoxBlur3x3Naive(const Image& src, Image& dst, std::size_t width, std::size_t height);

// 可分离：先水平 1x3 再垂直 3x1（两次一维）
void BoxBlurSeparable(const Image& src, Image& dst, std::size_t width, std::size_t height);

// 分块 separable，改善 cache
void BoxBlurSeparableTiled(const Image& src, Image& dst, std::size_t width, std::size_t height,
                           std::size_t tile);

}  // namespace perf14
