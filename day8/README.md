# Day8: 缓存层次与 Cache Line

目标：

- 理解 **L1/L2/L3** 与工作集大小的关系。
- 用 **行/列遍历** 量化 spatial locality 的收益。
- 知道 **cache line（通常 64B）** 是硬件预取与 false sharing 的基本单位（false sharing 在 Day 20 展开）。

预期学习时间：3–4h

---

# 一、理论要点

| 概念 | 要点 |
| --- | --- |
| Temporal locality | 刚访问的数据很可能再次被访问 |
| Spatial locality | 相邻地址很可能一起被访问 |
| Cache line | 一次加载进 cache 的最小单位（x86 常见 64B） |
| 工作集 | 程序某阶段活跃的数据量；超过某层 cache 容量 → miss 上升 |

用 `lscpu` 记录本机 L1d/L2/L3，填入 `day1/ENV.md`。当矩阵边长 `n` 增大时，`n²×4` 字节应和耗时拐点对照。

---

# 二、动手

```bash
cd day8
cmake -S . -B build -G Ninja && cmake --build build
./build/bench_cache --benchmark_repetitions=3
chmod +x scripts/run_perf_sweep.sh && ./scripts/run_perf_sweep.sh
```

观察：`n=64` 时 row/col 差距小；`n=2048` 时 col 明显更慢且 **cache-misses** 更高。

---

# 三、记录表

| n | 矩阵 KiB | Row ns | Col ns | Col/Row | col cache miss % |
| --- | --- | --- | --- | --- | --- |
| 64 | | | | | |
| 256 | | | | | |
| 1024 | | | | | |
| 2048 | | | | | |

---

# 四、Day 8 作业

1. 画出（或文字描述）当 `n` 从 64 增到 4096 时，**row/col 比值**如何变化，并与 L2/L3 容量对照。
2. 复现 Day 1 思考题：降到 `n=32` 预测 row≈col，并 benchmark 验证。
3. （选做）阅读 `SumStrided`，解释 stride 增大时对带宽利用的影响。
