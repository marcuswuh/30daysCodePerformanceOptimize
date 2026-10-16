#include "matmul.hpp"

#include <algorithm>
#include <cstring>

namespace perf10 {

Matrix MakeMatrix(std::size_t n, float fill) { return Matrix(n * n, fill); }

void MatMulIjk(const Matrix& a, const Matrix& b, Matrix& c, std::size_t n) {
  std::fill(c.begin(), c.end(), 0.0f);
  for (std::size_t i = 0; i < n; ++i)
    for (std::size_t j = 0; j < n; ++j)
      for (std::size_t k = 0; k < n; ++k)
        c[i * n + j] += a[i * n + k] * b[k * n + j];
}

void MatMulIkj(const Matrix& a, const Matrix& b, Matrix& c, std::size_t n) {
  std::fill(c.begin(), c.end(), 0.0f);
  for (std::size_t i = 0; i < n; ++i)
    for (std::size_t k = 0; k < n; ++k) {
      const float aik = a[i * n + k];
      for (std::size_t j = 0; j < n; ++j) c[i * n + j] += aik * b[k * n + j];
    }
}

void MatMulBlocked(const Matrix& a, const Matrix& b, Matrix& c, std::size_t n,
                   std::size_t bs) {
  std::fill(c.begin(), c.end(), 0.0f);
  for (std::size_t ii = 0; ii < n; ii += bs)
    for (std::size_t jj = 0; jj < n; jj += bs)
      for (std::size_t kk = 0; kk < n; kk += bs) {
        const std::size_t i_end = std::min(ii + bs, n);
        const std::size_t j_end = std::min(jj + bs, n);
        const std::size_t k_end = std::min(kk + bs, n);
        for (std::size_t i = ii; i < i_end; ++i)
          for (std::size_t k = kk; k < k_end; ++k) {
            const float aik = a[i * n + k];
            for (std::size_t j = jj; j < j_end; ++j) c[i * n + j] += aik * b[k * n + j];
          }
      }
}

void MatMulIkjUnroll4(const Matrix& a, const Matrix& b, Matrix& c, std::size_t n) {
  std::fill(c.begin(), c.end(), 0.0f);
  for (std::size_t i = 0; i < n; ++i)
    for (std::size_t k = 0; k < n; ++k) {
      const float aik = a[i * n + k];
      std::size_t j = 0;
      for (; j + 4 <= n; j += 4) {
        c[i * n + j + 0] += aik * b[k * n + j + 0];
        c[i * n + j + 1] += aik * b[k * n + j + 1];
        c[i * n + j + 2] += aik * b[k * n + j + 2];
        c[i * n + j + 3] += aik * b[k * n + j + 3];
      }
      for (; j < n; ++j) c[i * n + j] += aik * b[k * n + j];
    }
}

}  // namespace perf10
