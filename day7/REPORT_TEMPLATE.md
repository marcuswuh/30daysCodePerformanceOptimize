# 第 1 周性能分析报告

- 作者：
- 日期：
- 环境：见 `day1/ENV.md`

## 1. 项目与指标

- 被测程序：`day7/mini_grep_demo`
- 参数：`lines=`, `needle=`, 构建类型：
- 主要指标：wall time / items/s / 分配次数

## 2. Baseline（Google Benchmark 或 demo）

| 版本 | lines | time (median) | 备注 |
| --- | --- | --- | --- |
| slow | | | |
| fast | | | |

## 3. perf stat 归因

| 版本 | IPC | cache miss % | branch miss % |
| --- | --- | --- | --- |
| slow | | | |
| fast | | | |

**解读**（2–5 句）：

## 4. 热点（perf record / report）

Top 函数与 Overhead %：

1.
2.
3.

**调用栈说明**：

## 5. 指令级（perf annotate 或 objdump）

- 采样最高的源码行 / 汇编：
- 与 cache/分配 假设是否一致：

## 6. 内存（heaptrack / alloc 对比，可选）

| 版本 | 总分配次数 / peak |
| --- | --- |
| slow | |
| fast | |

## 7. 结论与后续

- 根因（一句话）：
- 已验证的优化效果（倍数）：
- 下一优先级（Day 8+）：
