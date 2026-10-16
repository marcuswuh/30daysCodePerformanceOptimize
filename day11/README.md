# Day11: 分支预测与无分支技巧

目标：

- 理解 **数据相关分支** 与 `branch-misses` 的关系。
- 掌握 **查表**、**无分支累加** 等适用场景与误用代价。
- 了解 `[[likely]]` / `[[unlikely]]`（C++20）仅对静态预测有提示作用，不能替代算法。

预期学习时间：3–4h

---

# 一、何时分支是问题

| 模式 | 分支行为 | 典型对策 |
| --- | --- | --- |
| 随机数据 `v > t` | 难预测 | 排序后扫描、无分支、分桶 |
| 偏斜分布（90% 走同一侧） | 预测器有效 | 保持分支，必要时 `[[likely]]` |
| 小范围离散值 | switch/jump table | 编译器常生成表；手写 LUT 对照 |

**注意**：无分支写法可能引入额外乘法与依赖链，未必更快——必须 benchmark。

---

# 二、运行

```bash
cd day11
cmake -S . -B build -G Ninja && cmake --build build
./build/bench_branch --benchmark_repetitions=3
chmod +x scripts/run_branch_miss.sh && ./scripts/run_branch_miss.sh
```

---

# 三、Day 11 作业

1. 对比 `SumBranch/random` vs `SumBranch/mostly_pass` 的 branch miss 率。
2. 解释为何 `mostly_pass` 时 Branch 与 Branchless 差距可能缩小甚至反转。
3. 在 CE 上查看 `SumIfBranchless` 是否生成 `cmov` 或仍保留分支。
