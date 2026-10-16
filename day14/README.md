# Day14: 第 2 周实战 — 图像盒式模糊

目标：综合运用 **局部性、循环/分块、算法降复杂度**，并用 **cache-misses** 验证。

预期学习时间：4–6h

---

# 一、三种实现

| 函数 | 复杂度（像素级） | 要点 |
| --- | --- | --- |
| `BoxBlur3x3Naive` | 每像素 9 次采样 | 重复读邻域，cache 不友好 |
| `BoxBlurSeparable` | 每像素 6 次一维采样 | 降算术量 + 顺序访问 |
| `BoxBlurSeparableTiled` | 同上，分块扫 | 提高 tmp/dst 的 cache 复用 |

2048×2048 单通道图约 4MiB，适合观察 L3 与带宽。

---

# 二、运行

```bash
cd day14
cmake -S . -B build -G Ninja && cmake --build build
./build/blur_demo 2048 2048 all
./build/bench_blur --benchmark_repetitions=3
chmod +x scripts/run_week2_capstone.sh && ./scripts/run_week2_capstone.sh
```

填写 `REPORT_TEMPLATE.md` 作为第 2 周总结。

---

# 三、Day 14 作业

1. 提交三版耗时与 **cache miss 率** 对比表。
2. 用 Day 4 的 `perf record` 看 naive 是否时间集中在 `At()` / 邻域循环。
3. 尝试 tile=32/64/128，记录最优值并解释。
4. （选做）与 Day 10 矩阵分块对比：图像行主序与分块的相同点。
