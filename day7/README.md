# Day7: 第 1 周综合练习 — mini_grep 性能剖析

目标：用第 1 周全套工具（Benchmark、perf stat/record/annotate、heaptrack）分析一个 **可复现的小项目**，写出结构化报告。

预期学习时间：4–6h

---

# 一、项目说明

`mini_grep` 模拟「在大量日志行里统计包含某子串的行数」：

| 实现 | 故意问题 | 优化方向 |
| --- | --- | --- |
| `CountLinesSlow` | 每行 `std::string` 拷贝 + 手写 O(n·m) 比较 | 去掉拷贝、用 `string_view::find` |
| `CountLinesFast` | 仍逐行扫描，但无额外堆分配 | 可进一步 SIMD / Aho-Corasick（超出本周） |

这不是真实 ripgrep，但足够练习 **定位热点 → 归因 → 验证** 的闭环。

---

# 二、构建与运行

```bash
cd day7
cmake -S . -B build -G Ninja
cmake --build build

./build/mini_grep_demo 500000 both ERROR
./build/bench_mini_grep --benchmark_repetitions=3
chmod +x scripts/run_profile_suite.sh
./scripts/run_profile_suite.sh
```

---

# 三、建议分析流程（对应 Day 1–6）

1. **Baseline**：`bench_mini_grep` 或 demo 的 wall time，记录 `ENV.md` 环境。
2. **perf stat**：对比 slow/fast 的 cycles、IPC、cache-misses（匹配阶段可用较小 lines 重复 run）。
3. **perf record + report**：确认热点在 `ContainsSubstring` / 拷贝路径。
4. **perf annotate**：看内层字符比较与 `memmove`（拷贝）占比。
5. **heaptrack**（Linux）：slow 是否出现大量短生命周期分配？（拷贝可能触发 SSO/堆）
6. **结论**：一次只改一处，fast 相对 slow 的倍数是否与工具链证据一致。

报告模板：`REPORT_TEMPLATE.md`

---

# 四、Day 7 作业（提交物）

1. 填写完整的 `REPORT_TEMPLATE.md`（可另存为 `reports/week1-<你的名字>.md`）。
2. 至少包含 **一张 perf report Top 栈** 或 **heaptrack 对比** 的文字说明。
3. 回答：若 needle 固定为 `"ERROR"` 且 lines 极大，下一周（Day 8+）你会从 **算法 / 缓存 / 分配** 哪一层继续优化？为什么？

---

# 五、延伸（选做）

- 将 needle 改为更短 / 更长，观察 slow 与 fast 差距如何变化。
- 对 `day6/mem_demo` 与 `mini_grep` 写一段对比：CPU 瓶颈 vs 分配瓶颈的识别差异。
