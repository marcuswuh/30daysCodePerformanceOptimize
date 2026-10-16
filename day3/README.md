# Day3: `perf stat` 与 CPU 性能计数器

目标：

- 会用 `perf stat` 采集 **cycles / instructions / cache / branch** 事件。
- 能从计数器推导 **IPC、cache miss 率、branch miss 率**，并关联到代码行为。
- 理解「算力受限 vs 内存受限 vs 分支受限」的粗粒度判断。

预期学习时间：3–4h

---

# 一、为什么需要硬件计数器

Google Benchmark 告诉你 **花了多少 wall time**，但不直接回答 **时间在等什么**：

| 现象 | 可能计数器信号 |
| --- | --- |
| 循环指令很少但很慢 | `cache-misses` 高 → 内存/布局 |
| 指令很多但 cycles 更多 | IPC 低 → 停顿（常是 cache 或依赖链） |
| 分支很多且 `branch-misses` 高 | 难预测分支 → 考虑无分支或排序数据 |

本日 workload 与 Day 1 的 `bench_traverse` 同源，并增加 **branchy** 模式便于对比 branch 事件。

---

# 二、工程结构

```
day3/
├── CMakeLists.txt
├── demo/workload_demo.cpp
├── src/workload.{hpp,cpp}
└── scripts/run_perf_stat.sh
```

构建：

```bash
cd day3
cmake -S . -B build -G Ninja
cmake --build build
./build/workload_demo 1024 row 8
```

参数：`./workload_demo <n> <row|col|branch> <repeat>`

---

# 三、`perf stat` 基本用法

```bash
chmod +x scripts/run_perf_stat.sh
./scripts/run_perf_stat.sh row 1024 8
./scripts/run_perf_stat.sh col 1024 8
./scripts/run_perf_stat.sh branch 4096 4
```

手动命令（与脚本等价）：

```bash
perf stat -e cycles,instructions,cache-references,cache-misses,branches,branch-misses \
  taskset -c 2 ./build/workload_demo 1024 col 8
```

## 3.1 指标解读

| 指标 | 计算 | 经验阈值（随 CPU 浮动） |
| --- | --- | --- |
| **IPC** | instructions / cycles | < 1 值得深挖；> 2 通常较健康 |
| **Cache miss 率** | cache-misses / cache-references | row vs col 应有明显差距 |
| **Branch miss 率** | branch-misses / branches | > 5% 常提示难预测分支 |

## 3.2 记录表（填自己的数据）

| Kind | IPC | cache miss % | branch miss % | time_ms |
| --- | --- | --- | --- | --- |
| row | | | | |
| col | | | | |
| branch | | | | |

---

# 四、Top-Down 与平台差异

Intel 可用 Brendan Gregg 的 **pmu-tools / toplev** 做 Top-Down；**AMD（Zen）** 请优先用 `perf list`、`perf stat -M`（若内核支持）或厂商工具。  
`perf record` / `annotate` 在两家平台均可用（Day 4–5）。

虚拟机 / 容器若看不到 `cycles`，请参考 Day 1 README 2.3，在物理机完成本日实验。

---

# 五、Day 3 作业

1. 对比 **row / col** 的 cache miss 率，用一段话解释与 Day 1 耗时比的关系。
2. 固定 `n=1024`，只改 `repeat`，观察 **cycles 是否近似线性**——理解「重复跑同一 workload」对 stat 的意义。
3. 把 `branch` 模式的 `threshold` 在源码里改成 `0` 或 `99`（几乎全过/全不过），再测 branch miss 率，写下预测与实测。
4. （选做）对 `day2` 的 `bench_sort_case` 跑同样的 `perf stat`，对比 sort 与矩阵遍历的瓶颈类型。
