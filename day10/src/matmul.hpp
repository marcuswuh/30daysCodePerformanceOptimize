#pragma once

#include <cstddef>
#include <vector>

namespace perf10 {

using Matrix = std::vector<float>;

Matrix MakeMatrix(std::size_t n, float fill);

// i-j-k：B 列访问不连续
void MatMulIjk(const Matrix& a, const Matrix& b, Matrix& c, std::size_t n);

// i-k-j：循环交换，改善 B 的访问局部性
void MatMulIkj(const Matrix& a, const Matrix& b, Matrix& c, std::size_t n);

// 分块：提高 L1/L2 复用
void MatMulBlocked(const Matrix& a, const Matrix& b, Matrix& c, std::size_t n,
                   std::size_t block_size);

// 手动 4 路展开（仅内层 k，教学用）
void MatMulIkjUnroll4(const Matrix& a, const Matrix& b, Matrix& c, std::size_t n);

}  // namespace perf10
