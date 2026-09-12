#include <benchmark/benchmark.h>

#include <cstddef>
#include <vector>

namespace {

int HeavySum(const std::vector<int>& data)
{
  int sum = 0;
  for (int value : data) // 计算平方和
  {
    sum += value * value;
  }
  return sum;
}

// 测量1
void BM_HeavySum_NoKeep(benchmark::State& state)
{
  const std::size_t n = static_cast<std::size_t>(state.range(0));
  const std::vector<int> data(n, 3); // 初始化数据

  for (auto _ : state)
  {
    HeavySum(data); // 计算平方和
  }
  state.SetItemsProcessed(static_cast<int64_t>(state.iterations()) *
                          static_cast<int64_t>(n));
}
BENCHMARK(BM_HeavySum_NoKeep)->Arg(1 << 20);

// 测量2
void BM_HeavySum_DoNotOptimize(benchmark::State& state) {
  const std::size_t n = static_cast<std::size_t>(state.range(0));
  const std::vector<int> data(n, 3);

  for (auto _ : state) {
    int sum = HeavySum(data);
    benchmark::DoNotOptimize(sum);
  }
  state.SetItemsProcessed(static_cast<int64_t>(state.iterations()) *
                          static_cast<int64_t>(n));
}
BENCHMARK(BM_HeavySum_DoNotOptimize)->Arg(1 << 20);

// 测量3
void BM_HeavySum_ClobberMemory(benchmark::State& state) {
  const std::size_t n = static_cast<std::size_t>(state.range(0));
  std::vector<int> data(n, 3);

  for (auto _ : state) {
    int sum = HeavySum(data);
    benchmark::DoNotOptimize(sum);
    // 避免编译器对内存做优化，当前场景加上这个可能会导致程序耗时略微增加。
    benchmark::ClobberMemory();
  }
  state.SetItemsProcessed(static_cast<int64_t>(state.iterations()) *
                          static_cast<int64_t>(n));
}
BENCHMARK(BM_HeavySum_ClobberMemory)->Arg(1 << 20);

}  // namespace

BENCHMARK_MAIN();
