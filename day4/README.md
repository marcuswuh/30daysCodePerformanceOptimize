# Day4: `perf record` + `perf report` 热点定位

目标：

- 用 **采样 profiling** 找到 CPU 时间占比最高的函数。
- 会读 **调用图**（`-g`）与 `perf report` 输出。
- （选做）用 **FlameGraph** 或 **hotspot** 可视化 `perf.data`。

预期学习时间：3–4h

---

# 一、从 stat 到 record

| 工具 | 回答问题 |
| --- | --- |
| `perf stat` | 整体瓶颈**类型**（cache? branch?） |
| `perf record` | **哪几个函数**占用了时间 |

采样 profiling 通过周期性中断记录 PC 与栈，开销通常 **1–5%**（频率 `-F` 越高越准但开销越大）。

---

# 二、快速开始

```bash
cd day4
chmod +x scripts/run_perf_record.sh scripts/run_flamegraph.sh
./scripts/run_perf_record.sh col 1024 12
```

默认跑 **列主序**（热点应在 `SumColMajor`）。再对比：

```bash
./scripts/run_perf_record.sh row 1024 12
```

在 `perf report` 里应看到 `SumColMajor` / `SumRowMajor` 占比差异。

---

# 三、命令详解

```bash
perf record -g -F 997 -o perf.data -- ./build/workload_demo 1024 col 12
perf report -i perf.data              # 交互 TUI
perf report -i perf.data --stdio      # 脚本友好
```

| 选项 | 含义 |
| --- | --- |
| `-g` | 记录调用栈（需 `-fno-omit-frame-pointer` 或 `--call-graph dwarf`） |
| `-F 997` | 每秒约 997 次采样（避开与定时器同频的锁步） |
| `-o perf.data` | 输出文件 |

## 3.1 图形化

- **hotspot**（KDE）：`hotspot perf.data`
- **FlameGraph**：见 `scripts/run_flamegraph.sh`（需 clone Brendan Gregg 仓库）

---

# 四、读 report 的检查清单

1. **Overhead %** 最高的符号是否与你预期的热点一致？
2. 若只有 `[unknown]` 或地址：是否用了 `RelWithDebInfo` / `-g`？
3. 调用栈是否完整：若只有一层，检查 `-fno-omit-frame-pointer`。
4. 对比 row vs col：热点函数名相同但 **Overhead 与 wall time** 应一致指向更慢的版本。

---

# 五、Day 4 作业

1. 提交 `perf report --stdio` 中 **Top 5** 符号（含 Overhead %）。
2. 对 `day2/build/bench_sort_case`（需先构建）做一次 `perf record`，找出排序热点是否在 libc 的 qsort/introsort 路径——写下命令与结论。
3. （选做）生成 `flamegraph.svg`，标出最宽的一帧对应哪个函数。
4. 思考题：`perf record` 采样的是 **CPU 时间** 还是 **wall time**？对 IO 阻塞程序有何局限？
