# Day6: 内存分析工具

目标：

- 理解「慢」有时来自 **分配次数 / 峰值 / 泄漏**，而不只是 CPU 算力。
- 会用 **heaptrack** 定位「谁在分配」。
- 会用 **valgrind massif** 看 **堆占用随时间** 的变化。
- 掌握一种 **轻量自定义追踪** 思路，并在本机跑通 demo。

预期学习时间：4h

---

# 一、为什么要单独做内存分析

Day 2–5 主要回答：**哪段代码最耗 CPU 时间**。  
Day 6 回答另一类问题：

| 现象 | 可能根因 | 典型工具 |
| --- | --- | --- |
| QPS 上不去、延迟抖动 | 热点路径上频繁 `malloc/free` | heaptrack |
| 进程 RSS 越跑越大 | 峰值过高、缓存未释放、泄漏 | massif / leaks |
| 小对象很多 | 容器增长、临时 `std::string` | heaptrack + 源码 |

**和 CPU profiling 的关系**：同一段「匹配前缀」逻辑，CPU  profile 可能显示时间花在 `strcmp` / 比较上；内存 profile 会显示 **每轮循环都在构造临时 string**——优化方向是 **去掉分配**，而不只是换比较函数。

本目录 demo 来自 Day 1 的简化版：

- **bad**：循环内 **完整拷贝** `std::string` 再比前缀 → 匹配阶段 **O(n) 次堆分配**（Item 名故意加长，避免 SSO 掩盖现象）
- **good**：`string_view` + `compare` → 匹配阶段 **0 次额外分配**

---

# 二、工程结构

```
day6/
├── CMakeLists.txt
├── demo/
│   ├── mem_demo.cpp           # 给 heaptrack / massif / Instruments 用
│   └── alloc_trace_demo.cpp   # 第四节：全局 new 计数（教学用）
├── src/
│   ├── match.hpp              # bad / good 实现（header-only）
│   └── alloc_counter.hpp      # 自定义分配计数（仅 demo）
└── scripts/
    ├── run_heaptrack.sh       # Linux
    ├── run_massif.sh          # Linux
    └── run_macos_malloc_log.sh
```

构建：

```bash
cd day6
cmake -S . -B build -DCMAKE_BUILD_TYPE=RelWithDebInfo
cmake --build build
./build/mem_demo 200000 both
./build/alloc_trace_demo 50000 both
```

`mem_demo` 参数：`./mem_demo <n> [bad|good|both]`

---

# 三、heaptrack：谁分配、分配多少次

## 3.1 安装（Linux）

```bash
sudo apt install -y heaptrack heaptrack-gui
heaptrack --version
```

macOS 课程路径见 **第七节**；heaptrack 非官方主力平台。

## 3.2 基本用法

```bash
chmod +x scripts/run_heaptrack.sh
./scripts/run_heaptrack.sh 100000 bad
./scripts/run_heaptrack.sh 100000 good
```

或手动：

```bash
heaptrack -o heaptrack.bad.gz ./build/mem_demo 100000 bad
heaptrack_gui heaptrack.bad.gz
```

## 3.3 在 GUI 里看什么

1. **Peak heap / Total allocations**：good 应显著低于 bad（同 n 下）。
2. **Flame Graph / Call Tree**：bad 应看到 `CountMatchesBad`、`std::string` 相关栈。
3. **Temporary allocations**：bad 路径上大量短生命周期分配是典型「优化信号」。

## 3.4 解读原则

- 先对比 **总分配次数**，再对比 **峰值**；有时次数降了但峰值仍高（大对象/容器 reserve）。
- 优化后应回到 **heaptrack 复测**，与 Day 2 的 benchmark 一样：**一次只改一处**。
- Debug 与 Release 分配模式可能不同；结论以 **RelWithDebInfo** 为准，与 Day 2 约定一致。

---

# 四、valgrind massif：堆占用随时间

## 4.1 安装与运行

```bash
sudo apt install -y valgrind
chmod +x scripts/run_massif.sh
./scripts/run_massif.sh 50000 bad
ms_print massif.out.bad | less
```

## 4.2 massif 擅长什么

| heaptrack | massif |
| --- | --- |
| 更偏 **分配事件 / 调用栈** | 更偏 **堆体积曲线**（时间轴） |
| 交互分析体验好 | 文本报告 + `ms_print` 足够做实验 |

关注输出中的 **peak memory** 和 **detailed snapshots**：程序是否在某一阶段抬升后未回落（可能正常缓存，也可能泄漏嫌疑）。

## 4.3 代价

Valgrind 会 **显著放慢** 程序（often 10x+），适合 **小 n** 的可复现实验，不适合线上或超大数据集。Day 6 脚本默认 `n=50000` 可酌情调小。

---

# 五、自定义分配追踪（入门）

正式环境应用 **heaptrack / ASan / 厂商 profiler**；本节 demo 展示 **「计数思路」**，便于无 Linux 工具时在 macOS 上自证。

## 5.1 实现要点

`src/alloc_counter.hpp` 在**单个翻译单元**内重载全局 `operator new/delete` 并计数。  
`operator new` 实现在 `src/alloc_counter.cpp`，并 **直接链进** `alloc_trace_demo`（见 `CMakeLists.txt`）。若放进静态库且未被显式引用，链接器可能 **丢弃** 钩子，导致计数恒为 0。

`alloc_trace_demo` 在每次测量前 `Reset()`，且 **不包含** `MakeItems` 的分配——只统计 **匹配阶段** 的 `new/delete`。

```bash
./build/alloc_trace_demo 50000 both
```

预期（量级示意，随 libc++/编译器略有差异）：

- **bad**：`new` ≈ **n**（本机示例 `n=10000` → `new=10000`）
- **good**：匹配阶段 **0** 次 `new`（`new=0`）

## 5.2 局限（必须知道）

- 全局 `operator new` 钩子 **不能** 替代 heaptrack（只覆盖包含该头的 TU；跨 .so 不全）。
- 多线程下计数是 atomic，但未做 per-thread 归因。
- **不要** 把此模式拷进生产库；仅教学。

更工程化的方向（Day 12 展开）：`std::pmr`、Arena、对象池 + 自定义 `allocator` 统计接口。

---

# 六、动手案例：bad → good 闭环

## 6.1 假设

- 固定 `n=100000`，`RelWithDebInfo` 构建。
- **bad** 比 **good** 慢，且 heaptrack 显示 bad 分配次数远高于 good。

## 6.2 记录表（请填自己的数据）

| 指标 | bad | good |
| --- | --- | --- |
| wall time (ms) | | |
| heaptrack 总分配次数 | | |
| massif peak heap (KB) | | |
| alloc_trace_demo `new=` | | |

## 6.3 结论模板

1. 根因是否为 **循环内临时 string**？（heaptrack 栈 + 源码 `CountMatchesBad`）
2. 优化后分配是否 **O(n) → O(0)**（匹配阶段）？
3. 若 CPU benchmark（可选 Day 2 技能）也做，时间是否与分配下降一致？

---

# 七、macOS 学员路径

本机若无 heaptrack/valgrind，按顺序：

```bash
chmod +x scripts/run_macos_malloc_log.sh
./scripts/run_macos_malloc_log.sh 50000 both
```

| 工具 | 作用 |
| --- | --- |
| `alloc_trace_demo` | 课程必做：分配次数对比 |
| `/usr/bin/time -l` | 看 `maximum resident set size` 粗略峰值 |
| **Instruments → Allocations** | 图形化看分配栈（推荐） |
| `leaks` | 进程退出后查泄漏（需 MallocStackLogging） |

Linux 实验请在虚拟机 / 云主机 / WSL2 完成 heaptrack + massif 部分；WSL2 上 valgrind 可用性因版本而异，以实测为准。

---

# 八、Day 6 作业

1. 跑通 `./build/mem_demo 200000 both`，确认 good 明显快于 bad。
2. **Linux**：提交 heaptrack 对比截图或文字（bad vs good 总分配次数）。
3. **macOS**：提交 `alloc_trace_demo` 输出 + Instruments 一张分配调用栈说明。
4. 用 `ms_print` 或 massif 曲线回答：peak 出现在程序哪一阶段？（构造 items vs 匹配）
5. 思考题：`CountMatchesBad` 若改成 `const std::string& prefix` 传参，分配会怎么变？为什么 good 仍更优？

---

# 九、常见问题

| 现象 | 原因 | 处理 |
| --- | --- | --- |
| heaptrack 找不到 | 未安装 / 非 Linux | 用 macOS 路径或 apt 安装 |
| massif 极慢 | Valgrind 插桩 | 减小 `n` |
| alloc_trace 计数为 0 | 钩子未链入 / SSO 掩盖 | 见 5.1；bad 路径需长字符串 |
| good/bad 时间差不多 | `n` 太小或 CPU 缓存掩盖 | 增大 `n` 或看分配次数 |

---

# 十、与后续课程衔接

- **Day 12**：对象池、Arena、`std::pmr` —— 在 Day 6 的「分配热点」上落地。
- **Day 2**：可用 Google Benchmark 给 `CountMatches*` 加固化性能回归。
- **Day 3–5**：若 CPU 仍慢但分配已少，再回 perf 查 cache / 分支。
