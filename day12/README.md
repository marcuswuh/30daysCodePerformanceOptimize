# Day12: 内存分配优化 — 池、Arena、std::pmr

目标：

- 识别 **频繁小分配** 对吞吐与延迟的影响（衔接 Day 6）。
- 实现 **对象池** 与 **`std::pmr::monotonic_buffer_resource`** 的典型用法。
- 知道 Arena 的生命周期约束：**整块释放**，不适合长期驻留的单个对象。

预期学习时间：4h

---

# 一、策略选择

| 手段 | 适用 | 限制 |
| --- | --- | --- |
| 对象池 | 固定大小对象、高频创建销毁 | 池大小、线程安全需设计 |
| monotonic_buffer / Arena | 请求/帧级批量分配 | 不能单独 free 中间对象 |
| `std::pmr` 容器 | 把分配器注入容器 | 需理解 resource 生命周期 |
| `reserve` / 复用 buffer | vector/string 增长 | 最简单，优先做 |

---

# 二、运行

```bash
cd day12
cmake -S . -B build -G Ninja && cmake --build build
./build/bench_alloc --benchmark_repetitions=3
./build/alloc_demo 50000
```

Linux：`chmod +x scripts/run_heaptrack.sh && ./scripts/run_heaptrack.sh`

---

# 三、Day 12 作业

1. 对比 `HeapList` vs `PoolList` 的时间与 heaptrack 分配次数。
2. 说明 `SumPmrVectors` 中 1MiB 缓冲区耗尽时会发生什么（如何扩展或改用 `unsynchronized_pool_resource`）。
3. 思考题：对象池在多线程下需要怎样同步？（预告 Day 19）
