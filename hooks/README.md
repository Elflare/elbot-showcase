# Hook Plugins Showcase

[ElBot Hook](https://github.com/Elflare/elbot/blob/main/docs/hooks.md) 可以通过规则 Hook 执行一次性脚本，也可以运行维护会话状态的持久 Hook。本目录的插件都使用 `hook.v2` JSON Lines 协议。

## 示例插件

| 插件 | 语言 | 类型 | 功能 | 触发方式 |
|------|------|------|------|----------|
| [what-to-eat](examples/what-to-eat/) | C | 一次性 | 随机抽取一道菜 | `吃什么` |
| [dice-roller](examples/dice-roller/) | C++ | 一次性 | 掷骰子，支持 NdM 语法 | `掷骰子` 或 `3d6` |
| [fortune-stick](examples/fortune-stick/) | TypeScript | 一次性 | 随机抽取一支签 | `求签` |
| [weather-demo](examples/weather-demo/) | Python | 持久 | 有状态天气助手 | `查询天气` |

## 安装

每个示例目录都有自己的 `hook.toml`。将所需目录复制到 ElBot 的 `plugins/`，编译一次性插件，并在根 `plugins/hooks.toml` 注册：

```toml
[[plugins]]
name = "what-to-eat"

[[plugins]]
name = "dice-roller"

[[plugins]]
name = "fortune-stick"

[[plugins]]
name = "weather-demo"
```

使用任意子集即可，只注册实际安装的插件。修改根配置后执行全局 `/hooks reload`。

### 一次性插件

在各插件目录中编译或运行：

```bash
# what-to-eat
cd plugins/what-to-eat
gcc -O2 -o what_to_eat what_to_eat.c

# dice-roller
cd plugins/dice-roller
g++ -std=c++17 -O2 -o dice_roller dice_roller.cpp

# fortune-stick
cd plugins/fortune-stick
ts-node fortune_stick.ts
# 或编译后运行
# tsc fortune_stick.ts && node fortune_stick.js
```

`hook.toml` 中的 `cwd = "."` 使命令在插件目录中执行，因此 C 示例可直接读取同目录的 `dishes.txt`。

`command` 必须是 argv 数组，例如 `command = ["ts-node", "fortune_stick.ts"]`；每个元素原样传递，不经过 shell。

每次规则命中时，ElBot 会启动一次性插件并依次请求：

```text
system.init -> event.handle
```

脚本对每个 request 在 stdout 写出一行 JSON response；日志只写 stderr。`event.handle` 的 result 使用 `status`、`outputs`、`consume`、`stop_propagation` 等 `hook.v2` 字段；本示例通过规则中的 `consume = true` 接管匹配消息。完成响应后，一次性脚本退出。

### 持久天气插件

`weather-demo` 使用 `uv run weather_hook.py` 启动常驻进程。它不调用 Host 工具，只演示 waiting 会话与后续输入路由。

全局 `/hooks reload` 后，可以依次测试：

```text
查询天气 上海
-> 上海天气：晴，25°C（测试数据）。

查询天气
-> 请输入城市。

北京
-> 请发送正确的城市，目前仅支持上海。

上海
-> 上海城市天气：晴，25°C（测试数据）。

/cancel
-> 在 waiting 期间取消当前 Hook 会话。
```

持久 Hook 先响应 `system.init`，随后持续处理 `event.handle`。在 waiting 期间，同一 platform、scope 和用户的后续消息会以 `continuation = true` 交给同一进程；Host 的 `/cancel` 会以 `event.cancel` 通知进程，不需要 response。

---

# Hook Plugins Showcase

[ElBot Hooks](https://github.com/Elflare/elbot/blob/main/docs/hooks.md) can run one-shot scripts through rule Hooks or keep a stateful Hook process alive. Every plugin in this directory uses the `hook.v2` JSON Lines protocol.

## Example Plugins

| Plugin | Language | Type | Feature | Trigger |
|--------|----------|------|---------|---------|
| [what-to-eat](examples/what-to-eat/) | C | One-shot | Pick a random dish | `吃什么` |
| [dice-roller](examples/dice-roller/) | C++ | One-shot | Roll dice with NdM notation | `掷骰子` or `3d6` |
| [fortune-stick](examples/fortune-stick/) | TypeScript | One-shot | Draw a random fortune stick | `求签` |
| [weather-demo](examples/weather-demo/) | Python | Stateful | Stateful weather assistant | `查询天气` |

## Installation

Each example directory contains its own `hook.toml`. Copy the directories you need into ElBot's `plugins/`, build the one-shot plugins, then register them in the root `plugins/hooks.toml`:

```toml
[[plugins]]
name = "what-to-eat"

[[plugins]]
name = "dice-roller"

[[plugins]]
name = "fortune-stick"

[[plugins]]
name = "weather-demo"
```

Register only the plugins you install. Run the global `/hooks reload` after changing the root configuration.

### One-Shot Plugins

Build or run each plugin from its plugin directory:

```bash
# what-to-eat
cd plugins/what-to-eat
gcc -O2 -o what_to_eat what_to_eat.c

# dice-roller
cd plugins/dice-roller
g++ -std=c++17 -O2 -o dice_roller dice_roller.cpp

# fortune-stick
cd plugins/fortune-stick
ts-node fortune_stick.ts
# Or compile before running:
# tsc fortune_stick.ts && node fortune_stick.js
```

`cwd = "."` in each `hook.toml` runs the command in its plugin directory, so the C example can read its colocated `dishes.txt` directly.

`command` must be an argv array, for example `command = ["ts-node", "fortune_stick.ts"]`. Each element is passed unchanged without a shell.

For every matching event, ElBot starts a one-shot plugin and sends requests in this order:

```text
system.init -> event.handle
```

The script writes one JSON response per request to stdout and logs only to stderr. The `event.handle` result uses `hook.v2` fields such as `status`, `outputs`, `consume`, and `stop_propagation`; these examples handle matching messages through `consume = true` in their rules. A one-shot plugin exits after its completed response.

### Stateful Weather Plugin

`weather-demo` starts a persistent process with `uv run weather_hook.py`. It makes no Host tool calls and demonstrates waiting sessions and continuation routing.

After a global `/hooks reload`, test it in this order:

```text
查询天气 上海
-> 上海天气：晴，25°C（测试数据）。

查询天气
-> 请输入城市。

北京
-> 请发送正确的城市，目前仅支持上海。

上海
-> 上海城市天气：晴，25°C（测试数据）。

/cancel
-> Cancels the active Hook session while waiting.
```

A stateful Hook responds to `system.init` and then continuously handles `event.handle`. While waiting, a later message from the same platform, scope, and user is delivered to the same process with `continuation = true`; the Host sends `/cancel` as an `event.cancel` notification, which needs no response.
