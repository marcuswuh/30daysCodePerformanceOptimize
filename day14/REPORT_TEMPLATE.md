# 第 2 周实战报告（Day 14）

- 作者：
- 环境：见 `day1/ENV.md`

## 1. 优化路径

| 版本 | 说明 | median time | cache miss % |
| --- | --- | --- | --- |
| Naive 3x3 | 二重循环 + 9 邻域 | | |
| Separable | 水平 + 垂直 1x3 | | |
| SeparableTiled | tile=64 | | |

## 2. 归因（结合 Day 8–11）

- 内存访问：
- 算法/操作次数：
- 是否还有必要做 SIMD（预告 Day 15–17）：

## 3. 与第 1 周工具链的对应

- Benchmark：
- perf stat：
- （可选）perf record 热点函数：

## 4. 结论与若继续优化

- 下一刀改什么：
- 预期收益与风险：
