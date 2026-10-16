# Day13: 字符串优化 — string_view、SSO、避免隐式转换

目标：

- 消除 **隐式拷贝** 与 **临时 string**（与 Day 6/7 呼应）。
- 理解 **SSO（Small String Optimization）** 的观测方式与平台差异。
- 用 **`reserve` + `append`** 替代循环 `operator+`。

预期学习时间：3h

---

# 一、检查清单

| 反模式 | 改法 |
| --- | --- |
| `void f(std::string s)` | `string_view` / `const string&` |
| 循环内 `substr` / `+` | `compare` / 预分配 |
| 用 `string` 做字典 key 临时量 | `string_view` 或 `string` 驻留 |
| 忽略 SSO 导致 benchmark 失真 | 长字符串（>SSO）做分配实验 |

---

# 二、运行

```bash
cd day13
cmake -S . -B build -G Ninja && cmake --build build
./build/bench_strings --benchmark_repetitions=3
./build/sso_demo
```

---

# 三、Day 13 作业

1. 对比 `CountBad` vs `CountGood` 的时间；Linux 上补 heaptrack 分配次数。
2. 修改 `MakeLines` 的 `suffix_len` 为 0/8/64，观察 SSO 对 bad 路径的影响。
3. 列出你项目里 3 个可改为 `string_view` 的 API 签名（不必改代码，写建议即可）。
