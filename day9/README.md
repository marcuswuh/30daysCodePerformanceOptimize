# Day9: 数据布局 — AoS / SoA / 冷热分离

目标：

- 对比 **Array of Structs** 与 **Struct of Arrays** 在「只更新部分字段」时的 cache 行为。
- 理解 **字段顺序** 与 padding 如何浪费 cache line。
- 能说出何时保留 AoS（语义清晰、字段总是一起访问）。

预期学习时间：3–4h

---

# 一、核心对比

| 布局 | 优点 | 典型代价 |
| --- | --- | --- |
| AoS | 对象语义自然、局部性好（整对象一起用） | 只读 hot 字段时加载 cold 字段 |
| SoA | SIMD/顺序访问友好、hot 数据紧凑 | 改一个「对象」需跨多个数组 |
| 冷热分离 | 同 AoS 语义下减少 line 浪费 | 设计/重构成本 |

---

# 二、运行

```bash
cd day9
cmake -S . -B build -G Ninja && cmake --build build
./build/bench_layout --benchmark_repetitions=3
```

用 `perf stat` 对比 `IntegrateAoS/1000000` 与 `IntegrateSoA/1000000` 的 cache-misses。

---

# 三、Day 9 作业

1. 记录 AoS vs SoA 在 `n=1'000'000` 的 **items/s** 与 cache miss 率。
2. 用 `sizeof(EntityHotColdBad)` 与 `sizeof(EntityHotColdGood)` 解释 Sum 基准差异（可配合 `pahole` 或编译器输出）。
3. 思考题：若每帧既要积分又要读 `tag` 做渲染，SoA 还合适吗？如何折中？
