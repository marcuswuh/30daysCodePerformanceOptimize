# Day2: BenchMark

目标：

- 了解 CMake 工具链在编译时会帮我们做哪些优化。
- 知道 Google Benchmark 如何帮助我们**正确**测量性能。
- 完成一个可复现、可解释的性能测试案例。

预期学习时间：4h

---

# 一、编译器自动优化

性能测量必须建立在**真实的运行时代码**之上。编译器在 `-O2`/`-O3` 下会做大量变换；若不理解这些变换，基准测试可能测到的是"编译器删掉的空循环"，而不是你的算法。

## 1.1 CMake 如何控制优化级别

CMake 通过 `CMAKE_BUILD_TYPE` 决定默认编译选项。本课程约定：


| `CMAKE_BUILD_TYPE` | 典型选项           | 用途                |
| ------------------ | -------------- | ----------------- |
| `Debug`            | `-O0 -g`       | 调试逻辑，**不能**用于性能结论 |
| `RelWithDebInfo`   | `-O2 -g`       | 日常性能测试（推荐默认）      |
| `Release`          | `-O3 -DNDEBUG` | 发布/极限性能对比         |
| `MinSizeRel`       | `-Os`          | 体积优先场景            |


查看某个 target 实际使用的 flags：

```bash
# 在build里面写 编译规则缓存，要求规则里面配RelWithDebInfo的编译要求
cmake -S . -B build -DCMAKE_BUILD_TYPE=RelWithDebInfo
# 编译并输出详细信息，find info with "-O2"
cmake --build build --verbose 2>&1 | head -30   # 观察 g++/clang++ 命令行
```

也可以对单个 target 追加选项（不影响全局）：

```cmake
target_compile_options(bench_dce PRIVATE -O3 -march=native)
```



## 1.2 编译器常见优化（与基准测试强相关）


| 优化              | 做了什么            | 对测量的影响                     |
| --------------- | --------------- | -------------------------- |
| **死代码消除（DCE）**  | 结果未被使用 → 整段计算删除 | 测出 0 ns 假象                 |
| **常量传播 / 常量折叠** | 编译期算出已知结果       | 循环体被替换成常数                  |
| **循环展开**        | 减少分支、提高 ILP     | 同样算法，`-O0` 与 `-O2` 差数倍     |
| **自动向量化**       | 用 SIMD 一次处理多个元素 | `-O3 -march=native` 可能再快一截 |
| **内联**          | 小函数展开到调用点       | 消除 call 开销，便于进一步优化         |
| **LTO（链接时优化）**  | 跨翻译单元内联/删除      | 需要 `-flto`，Day 16 展开       |


**核心结论**：微基准里若被测代码"算完就扔"，编译器有权认为它无副作用并删除——这不是 Benchmark 的 bug，而是测量方法错误。

## 1.3 动手：对比 Debug / RelWithDebInfo / Release

本目录提供了 `bench/bench_dce.cpp` 与脚本 `scripts/run_opt_compare.sh`：

```bash
cd day2
chmod +x scripts/run_opt_compare.sh
./scripts/run_opt_compare.sh bench_dce 'BM_HeavySum.*'
```

三个用例对比：

1. `BM_HeavySum_NoKeep`：返回值未使用 → Release 下可能接近 0 ns
2. `BM_HeavySum_DoNotOptimize`：用 `DoNotOptimize` 钉住结果 → 反映真实计算成本
3. `BM_HeavySum_ClobberMemory`：额外 `ClobberMemory()`，防止跨迭代复用寄存器中的"幽灵结果"

**预期现象（Apple Silicon / x86 类似，具体数值因机器而异）**：


| 构建类型           | `NoKeep`    | `DoNotOptimize`          |
| -------------- | ----------- | ------------------------ |
| Debug          | 较慢          | 较慢                       |
| RelWithDebInfo | 可能极快（被 DCE） | 正常（毫秒级）                  |
| Release        | 可能极快（被 DCE） | 正常，且常比 RelWithDebInfo 再快 |




## 1.4 用汇编验证"代码是否还在"

在线 [Compiler Explorer](https://godbolt.org) 或本机：

```bash
cmake --build build-Release
objdump -d build-Release/bench_dce | less
# or
objdump -d build-Debug/bench_dce | grep -c "mul"
objdump -d build-Release/bench_dce | grep -c "mul"
```

观察 `HeavySum` 在 `-O0` 与 `-O2` 下的指令条数差异。若 `NoKeep` 路径里循环体消失，说明 DCE 生效——这解释了为何必须 `DoNotOptimize`。

## 1.5 本节小结

- 性能测试默认用 `RelWithDebInfo` 或 `Release`，并保留 `-g` 以便后续用 profiler 定位。
- **永远不要**在 `Debug` 构建上讨论"谁更快"。
- 被测代码的结果必须通过 `benchmark::DoNotOptimize`（或等效手段）逃逸出编译器分析，否则测到的可能是优化器，而不是你的代码。

---



# 二、Google Benchmark 介绍

Google Benchmark 是 C++ 微基准测试库：自动选择迭代次数、统计耗时、输出人类可读的报告。Day 1 已安装；本节聚焦**正确用法**。

## 2.1 最小结构

```cpp
#include <benchmark/benchmark.h>

static void BM_Example(benchmark::State& state)
{
  for (auto _ : state)
  { // 计时代码必须放在这个循环里
    // 被测代码
  }
}
BENCHMARK(BM_Example);

BENCHMARK_MAIN();                 // 提供 main()；也可链接 benchmark_main
```

构建（macOS 已 `brew install google-benchmark` 时）：

```bash
cmake -S . -B build          # 有 Ninja 时可加 -G Ninja
cmake --build build
./build/bench_dce
```



## 2.2 防止测量失真：`DoNotOptimize` 与 `ClobberMemory`

```cpp
int sum = Compute();
benchmark::DoNotOptimize(sum);      // 告诉编译器：这个值"可能被读到"
benchmark::ClobberMemory();         // 清空编译器对内存的假设
```


| API                | 何时用                      |
| ------------------ | ------------------------ |
| `DoNotOptimize(x)` | 几乎总是需要，钉住计算结果            |
| `ClobberMemory()`  | 怀疑编译器跨迭代复用寄存器、或测试内存读写时使用 |




## 2.3 把准备工作移出计时代码：`PauseTiming` / `ResumeTiming`

数据生成、拷贝、分配不应算进"被测算法"的时间：

```cpp
for (auto _ : state)
{
  state.PauseTiming();
  auto data = MakeInput(n);         // 不计时
  state.ResumeTiming();

  Sort(data);                       // 只计这部分
  benchmark::DoNotOptimize(data.data());
}
```

`src/bench_sort_case.cpp` 中所有排序基准都遵循这一模式。

## 2.4 参数化与复杂度分析

```cpp
void BM_Foo(benchmark::State& state) {
  const int64_t n = state.range(0);  // 读注册时传入的第 0 个参数
  for (auto _ : state) { /* 用 n 控制数据规模 */ }
}

// 【一维 Arg】链式写多个 Arg，等价于「同一份 BM_Foo 逻辑，用不同参数各跑一遍」。
// - 每个 Arg(x) 生成一条独立 benchmark，名称形如 BM_Foo/1000、BM_Foo/10000。
// - 函数内一律用 state.range(0) 取当前这次运行的那个 x（只有一维时 index 固定为 0）。
// - 两次运行互不影响：Google Benchmark 会分别计时、分别输出一行结果。
BENCHMARK(BM_Foo)->Arg(1000)->Arg(10000);

void BM_Bar(benchmark::State& state) {
  const int64_t n = state.range(0);       // 第 0 维，此处为 1000
  const int64_t threads = state.range(1); // 第 1 维，此处为 4
  (void)threads;
  for (auto _ : state) { /* ... */ }
}

// 【多维 Args】注意 API 是 Args（复数），参数是 initializer_list。
// - Args({1000, 4}) 表示这一次运行里：state.range(0)==1000 且 state.range(1)==4。
// - 链式 ->Args({1000, 4})->Args({10000, 8}) 会注册两条 case，名称类似 BM_Bar/1000/4 与 BM_Bar/10000/8（以实际输出为准）。
// - 适合扫「规模 × 线程数」「规模 × 重复因子」等二维实验；维数再往上就用 range(2)、range(3)…
BENCHMARK(BM_Bar)->Args({1000, 4})->Args({10000, 8});

// 【Range + RangeMultiplier】用几何级数自动扫参，省去手写一串 Arg。
// - Range(1000, 100000) 表示 state.range(0) 从 1000 扫到 100000（含端点，具体规则见库文档）。
// - RangeMultiplier(10) 表示每一步 ×10：1000 → 10000 → 100000，共 3 个 case。
// - 效果类似 ->Arg(1000)->Arg(10000)->Arg(100000)，但规模变大时更易维护。
// - 可与 Arg/Args 混用在同一条链上（不常见）；本课程在 bench_sort_case 里用 Range 扫排序规模 n。
BENCHMARK(BM_Foo)->Range(1000, 100000)->RangeMultiplier(10);

void BM_FooWithComplexity(benchmark::State& state) {
  const int64_t n = state.range(0);
  for (auto _ : state) { /* 耗时随 n 变化，例如 O(n log n) 的排序 */ }
  state.SetComplexityN(n);  // 告诉库「当前这次运行的自变量是 n」，拟合才有意义
}

// 【Complexity】在所有 Range/Arg 跑完后，额外输出 BigO 拟合行（如 BigO、RMS%）。
// - ->Complexity(benchmark::oNLogN) 表示预期耗时 ∝ n·log(n)；常见还有 oN、oNSquared、o1 等。
// - 必须在循环外调用 state.SetComplexityN(n)（n 为规模），否则库不知道 x 轴是什么。
// - 输出里的 BigO 系数/RMS 用于检查「实测曲线是否像声明的复杂度」；RMS 很大说明假设不对或测量噪声大。
// - 本仓库：StdSort 用 oNLogN，BubbleSort 用 oN（实际是 O(n²)，用 oN 拟合会偏差，见案例讨论）。
BENCHMARK(BM_FooWithComplexity)
    ->Range(1000, 100000)
    ->RangeMultiplier(10)
    ->Complexity(benchmark::oNLogN);
```

运行时可筛选：

```bash
./build/bench_sort_case --benchmark_filter=StdSort/random
./build/bench_sort_case --benchmark_repetitions=5 \
  --benchmark_report_aggregates_only=true
```

常用命令行：


| 参数                          | 作用                     |
| --------------------------- | ---------------------- |
| `--benchmark_filter=Regex`  | 只跑匹配的 case             |
| `--benchmark_repetitions=N` | 重复 N 次，输出 median/mean  |
| `--benchmark_min_time=0.5s` | 每个 case 至少跑够 0.5s，降低噪声 |
| `--benchmark_format=json`   | 输出 JSON，便于 CI 对比       |




## 2.5 自定义指标：`SetItemsProcessed` / `SetBytesProcessed`

```cpp
state.SetItemsProcessed(iterations * n);           // 显示 items/s
state.SetBytesProcessed(iterations * n * sizeof(T)); // 显示 bytes/s
```

这让不同实现可以在**吞吐**维度对比，而不仅是 wall time。

## 2.6 写出可信基准的检查清单

1. **构建类型**：`RelWithDebInfo` 或 `Release`
2. **结果逃逸**：`DoNotOptimize` 被测输出
3. **计时边界**：分配/填充在 `PauseTiming` 内
4. **输入真实**：随机、有序、逆序、近似有序等分布都要测（见第三节）
5. **环境稳定**：笔记本插电；Linux 上固定 `performance` governor 并绑核（`taskset -c 2`）
6. **重复与方差**：看 `cv`（变异系数），`cv > 5%` 时谨慎解读小幅提升
7. **一次改一处**：对比两个版本时，只改被测逻辑，其余保持一致



## 2.7 本节小结

Google Benchmark 解决的是"如何计时与统计"；**正确性**仍取决于你是否避免了 DCE、是否把 setup 排除在计时外、是否用了有代表性的数据。下一节把这些原则串成一个完整案例。

---



# 三、性能测试案例



## 3.1 案例背景

**问题**：对 `std::vector<int>` 排序，`std::sort`（内省排序，平均 O(n log n)）与手写冒泡排序（O(n²)）在不同规模、不同输入分布下各表现如何？

**为什么选排序**：

- 算法复杂度差异大，Benchmark 的 `--benchmark_complexity` 能直观验证
- 输入分布（随机 / 已有序 / 逆序 / 近似有序）会显著影响现代 sort 的实现路径
- 代码短，容易把精力放在**测量方法**而非业务逻辑



## 3.2 工程结构

```
day2/
├── CMakeLists.txt
├── bench/
│   ├── bench_dce.cpp          # 第一节：DCE 与优化级别
│   └── bench_sort_case.cpp    # 第三节：排序案例
├── src/
│   ├── sort.hpp
│   └── sort.cpp
└── scripts/
    └── run_opt_compare.sh
```



## 3.3 运行案例

```bash
cmake -S . -B build -G Ninja
cmake --build build

# 完整跑一遍（含复杂度拟合，耗时稍长）
./build/bench_sort_case

# 只看 std::sort + 随机输入
./build/bench_sort_case --benchmark_filter='StdSort/random'

# 冒泡排序只在小规模 Arg 上注册（100~1000）
./build/bench_sort_case --benchmark_filter=BubbleSort
```



## 3.4 如何读结果

示例输出（数值仅为示意）：

```
StdSort/random/1000          85.2 us    items_per_second=11.7M
StdSort/random/10000        1.12 ms    items_per_second=8.9M
StdSort/sorted/10000        0.31 ms    items_per_second=32.1M   ← 已有序往往更快
StdSort/reversed/10000      0.98 ms
BubbleSort/1000             2.40 ms    items_per_second=416k
```

**应得出的结论**（而非只看谁数字小）：

1. **同算法、不同分布**：`sorted` / `nearly_sorted` 常比 `random` 快，说明不能只用一种输入下结论。
2. **复杂度曲线**：`StdSort` 应接近 O(n log n)；`BubbleSort` 接近 O(n²)——若拟合斜率不对，检查是否把 setup 计进了时间。
3. **算法选型**：n=1000 时冒泡尚可接受；n=10000 时差距 orders of magnitude——微基准的价值是**在正确规模上**做决策。



## 3.5 案例中的最佳实践对照


| 实践        | 在本案例中的体现                                      |
| --------- | --------------------------------------------- |
| Setup 不计时 | `PauseTiming` / `ResumeTiming` 包裹 `MakeInput` |
| 防 DCE     | `DoNotOptimize(data.data())`                  |
| 参数扫描      | `Range(1000, 100000)` + `RangeMultiplier(10)` |
| 多分布       | `Random / Sorted / Reversed / NearlySorted`   |
| 复杂度验证     | `->Complexity(benchmark::oNLogN)`             |
| 避免无意义对比   | 冒泡只注册到 n≤1000，不对 10 万元素硬跑                     |




## 3.6 延伸实验（建议亲自完成）

1. **编译级别**：分别用 `Release` 与 `Debug` 跑 `bench_sort_case`，记录 `StdSort/random/10000` 的比值。
2. **固定规模看分布**：固定 `n=10000`，比较四种 pattern 的 median 与 `cv`。
3. **（Linux）叠加 perf**：`perf stat -e cycles,cache-misses ./build/bench_sort_case --benchmark_filter=StdSort/random/10000`，为 Day 3 预热。
4. **改一个变量**：把 `int` 换成 `std::string` 再测，观察 items/s 与 bytes/s 哪个更有解释力。



## 3.7 Day 2 作业

1. 跑通 `./scripts/run_opt_compare.sh`，截图或记录 `NoKeep` vs `DoNotOptimize` 在 Release 下的差异，并用一句话解释原因。
2. 跑通 `bench_sort_case`，填写下表（填你自己的 median 数据）：


| Case           | n     | Time (median) | items/s | 备注  |
| -------------- | ----- | ------------- | ------- | --- |
| StdSort/random | 10000 |               |         |     |
| StdSort/sorted | 10000 |               |         |     |
| BubbleSort     | 1000  |               |         |     |


1. 思考题：若把 `DoNotOptimize(data.data())` 去掉，`StdSort` 的结果会怎样？实测验证你的猜测。
2. （选做）为 `StdSort` 增加 `Args({100000})`，观察是否触发内存带宽瓶颈；结合 Day 8 缓存内容预判 L3 行为。

---



# 附录：



## macOS 与 Linux 差异


| 项目           | macOS                           | Linux                                         |
| ------------ | ------------------------------- | --------------------------------------------- |
| 安装 Benchmark | `brew install google-benchmark` | `apt install libbenchmark-dev` 或 FetchContent |
| 绑核           | 无 `taskset`；可忽略或安装 `cpuset`     | `taskset -c 2 ./build/...`                    |
| perf         | 支持有限                            | 完整 PMU（见 Day 1）                               |
| 频率固定         | 系统设置 → 电源 → 高性能                 | `cpupower frequency-set -g performance`       |


无论平台，**构建类型 + DoNotOptimize + 代表性输入** 这三条原则不变。

## 编译器识别DCE的逻辑

> [!Note]
>
>     若一段代码的执行结果，不会改变程序在语言标准意义上的「可观察行为」，编译器在优化级别足够高时就可以把它删掉。
>
> 通常地，编译器认为必须保留：
>
> - 写 volatile
>
> - IO，如cout，系统调用等等
> - 内存对外可见的写入
> - 经过 DoNotOptimize, asm barrier标记的代码段
>
> 可以删掉的情况：
>
> - 通过一个函数得到返回值，但是调用点后续没有用这个返回值。
> - 纯计算链路，但是被证明不影响最终可观察状态。
> - -O2/O3 下，发生 内联 + 常量折叠（即有些表达式在编译阶段就一定能得到，那么这些表达式会被直接被替换成常量/立即数）。

