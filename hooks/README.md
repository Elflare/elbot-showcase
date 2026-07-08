# Hook Plugins Showcase

ElBot [Hook](https://github.com/Elflare/elbot/blob/main/docs/hooks.md) 层支持通过 `exec` action 执行本地脚本。脚本遵循 `hook.v1` 行协议：ElBot 向 stdin 写一行 init JSON，脚本向 stdout 每行写 JSON frame，最后写 `done` 或 `error` frame。

本 showcase 用三种语言编写功能各异的 hook 插件，展示 ElBot 对多语言的包容性。

## 示例插件

| 插件 | 语言 | 功能 | 触发方式 |
|------|------|------|----------|
| [what-to-eat](examples/what-to-eat/) | C | 随机抽取一道菜 | 说“吃什么” |
| [dice-roller](examples/dice-roller/) | C++ | 掷骰子，支持 NdM 语法 | 说“掷骰子”或 `3d6` |
| [fortune-stick](examples/fortune-stick/) | TypeScript | 求签，随机抽一支签 | 说“求签” |

## 通用协议要点

- 脚本只读 stdin **第一行**（init frame），不要 read-to-end。
- stdout 每行一个 JSON frame：`output` → `done`（或 `error`）。
- `output` frame 的 `outputs` 字段是 output segment 数组，支持 `text`/`image`/`file`/`emoticon` 等类型。
- stderr 不参与协议，仅用于日志。
- 脚本写出 `done`/`error` 后应以 exit code 0 退出。

## 安装方式

1. 编译源码（TS 需 `tsc` 或 `ts-node`）：

```bash
# C
gcc -O2 -o what_to_eat what_to_eat.c

# C++
g++ -std=c++17 -O2 -o dice_roller dice_roller.cpp

# TypeScript (需要 Node.js)
ts-node fortune_stick.ts
# 或编译后运行
tsc fortune_stick.ts && node fortune_stick.js
```

2. 将编译产物和数据文件放到 ElBot `plugins/` 对应子目录。

3. 在 `plugins/hooks.toml` 中添加规则：

```toml
# 今天吃什么（C）
[[rules]]
name = "what_to_eat"
on = "platform.message.received"
priority = 100
if = "message.input_text"
op = "fullmatch"
value = "吃什么"
consume = true

[[rules.actions]]
type = "exec"
name = "eat"
command = "./what-to-eat/what_to_eat"
timeout_seconds = 5

# 掷骰子（C++）
[[rules]]
name = "dice_roller"
on = "platform.message.received"
priority = 100
if = "message.input_text"
op = "regex"
value = "^(\\d+)?d(\\d+)$|掷骰子|摇骰子|掷个骰子|摇个骰子"
consume = true

[[rules.actions]]
type = "exec"
name = "roll"
command = "./dice-roller/dice_roller"
timeout_seconds = 5

# 求签（TypeScript）
[[rules]]
name = "fortune_stick"
on = "platform.message.received"
priority = 100
if = "message.input_text"
op = "fullmatch"
value = "求签"
consume = true

[[rules.actions]]
type = "exec"
name = "fortune"
command = "ts-node ./fortune-stick/fortune_stick.ts"
timeout_seconds = 10
```

4. 重启 ElBot 或等待自动 reload。

## 各语言特性展示

### C — what-to-eat

- 手动内存操作（`char[][]` 固定大小数组）
- `stdio` 流读写（`fgets`/`fopen`/`printf`）
- 手写 JSON escape 函数
- `time.h` + `srand`/`rand` 随机数
- 零外部依赖，编译产物体积极小

### C++ — dice-roller

- `<random>` 梅森旋转引擎（`std::mt19937`）+ `std::uniform_int_distribution`
- `<chrono>` 高精度时钟作为随机种子
- `<regex>` 正则解析 NdM 语法
- `class`/`struct` 封装掷骰逻辑
- `std::vector` + 范围 for 循环
- `std::ostringstream` 流式拼接输出

### TypeScript — fortune-stick

- `enum` / `interface` 类型系统
- `async`/`await` + `readline` 异步读 stdin
- 模板字符串拼接签文
- `Promise` 封装 readline 读取
- 数据驱动的签文数组设计

---

# Hook Plugins Showcase (English)

ElBot [Hook](https://github.com/Elflare/elbot/blob/main/docs/hooks.md) layer supports executing local scripts via `exec` action. Scripts follow the `hook.v1` line protocol: ElBot writes one init JSON line to stdin, scripts write JSON frames line-by-line to stdout, ending with `done` or `error`.

This showcase demonstrates three plugins in different languages to show ElBot's language-agnostic extensibility.

## Example Plugins

| Plugin | Language | Feature | Trigger |
|--------|----------|---------|---------|
| [what-to-eat](examples/what-to-eat/) | C | Random dish picker | Say "what to eat" |
| [dice-roller](examples/dice-roller/) | C++ | Dice roller with NdM syntax | Say "roll dice" or `3d6` |
| [fortune-stick](examples/fortune-stick/) | TypeScript | Fortune stick divination | Say "draw fortune" |

## Protocol Summary

- Read only the **first line** of stdin (init frame); do not read-to-end.
- Write JSON frames to stdout, one per line: `output` → `done` (or `error`).
- The `output` frame's `outputs` field is an array of output segments supporting `text`/`image`/`file`/`emoticon`.
- stderr is not part of the protocol; used for logging only.
- Exit with code 0 after writing `done`/`error`.

## Installation

1. Compile source (TS requires `tsc` or `ts-node`):

```bash
# C
gcc -O2 -o what_to_eat what_to_eat.c

# C++
g++ -std=c++17 -O2 -o dice_roller dice_roller.cpp

# TypeScript (requires Node.js)
ts-node fortune_stick.ts
# or compile then run
tsc fortune_stick.ts && node fortune_stick.js
```

2. Place compiled binaries and data files into ElBot `plugins/` subdirectories.

3. Add rules to `plugins/hooks.toml`:

```toml
# What to eat (C)
[[rules]]
name = "what_to_eat"
on = "platform.message.received"
priority = 100
if = "message.input_text"
op = "fullmatch"
value = "吃什么"
consume = true

[[rules.actions]]
type = "exec"
name = "eat"
command = "./what-to-eat/what_to_eat"
timeout_seconds = 5

# Dice roller (C++)
[[rules]]
name = "dice_roller"
on = "platform.message.received"
priority = 100
if = "message.input_text"
op = "regex"
value = "^(\\d+)?d(\\d+)$|掷骰子|摇骰子|掷个骰子|摇个骰子"
consume = true

[[rules.actions]]
type = "exec"
name = "roll"
command = "./dice-roller/dice_roller"
timeout_seconds = 5

# Fortune stick (TypeScript)
[[rules]]
name = "fortune_stick"
on = "platform.message.received"
priority = 100
if = "message.input_text"
op = "fullmatch"
value = "求签"
consume = true

[[rules.actions]]
type = "exec"
name = "fortune"
command = "ts-node ./fortune-stick/fortune_stick.ts"
timeout_seconds = 10
```

4. Restart ElBot or wait for auto reload.

## Language Feature Highlights

### C — what-to-eat

- Manual memory management (`char[][]` fixed-size arrays)
- `stdio` stream I/O (`fgets`/`fopen`/`printf`)
- Hand-written JSON escape function
- `time.h` + `srand`/`rand` for randomness
- Zero external dependencies, minimal binary size

### C++ — dice-roller

- `<random>` Mersenne Twister engine (`std::mt19937`) + `std::uniform_int_distribution`
- `<chrono>` high-resolution clock as random seed
- `<regex>` for parsing NdM notation
- `class`/`struct` encapsulation
- `std::vector` + range-based for loops
- `std::ostringstream` for stream formatting

### TypeScript — fortune-stick

- `enum` / `interface` type system
- `async`/`await` with `readline` for async stdin reading
- Template literals for string interpolation
- `Promise` wrapper for readline
- Data-driven fortune array design
