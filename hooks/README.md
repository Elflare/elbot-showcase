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

4. 重启 ElBot 或者使用 `/hooks reload` 重载。


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

4. Restart ElBot or run `/hooks reload`.

