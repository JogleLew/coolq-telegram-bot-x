# CoolQ Telegram Bot X

Language: 🇺🇸 [English](https://github.com/JogleLew/coolq-telegram-bot-x/blob/master/README.md) | 🇨🇳 简体中文

CoolQ Telegram Bot X 是一个消息转发机器人。它能够实现 [QQ](https://im.qq.com/) / [Telegram ](https://telegram.org/)群组之间的消息转发。仅需简单的配置，就能方便的定制属于自己的转发逻辑。想要链接 QQ 群和 Telegram 群组，或者合并多个群组的消息？一个CoolQ Telegram Bot X 就能满足你。

## 机器人特性

#### 深度集成

CoolQ Telegram Bot X 集成了 QQ 机器人和 Telegram 机器人。**[酷 Q](https://cqp.cc/) Message SDK** 保证了每一条 QQ 消息都能稳定和可靠的接收和发送。基于官方 [Telegram Bot API](https://core.telegram.org/bots/api) 的 **Telegram C++ SDK** ，强力驱动收发 Telegram 的多种消息。消息转发逻辑处理单元会根据转发配置，将 QQ 机器人或 Telegram 机器人接收到的消息转发到相应的群组。

两个消息处理 SDK 和消息转发逻辑处理单元均被打包进一个 **酷 Q 插件**。启动酷 Q，启用 CoolQ Telegram Bot X 插件，一切就是这么简单 :)

#### 多向转发

CoolQ Telegram Bot X 支持多种转发类型。不管是 **QQ 转发到 Telegram**，还是 **Telegram 转发到 QQ**，都能正确的解析和转发消息。当然了， **QQ 转发到 QQ** 以及 **Telegram 转发到 Telegram** 同样支持，帮助你管理和合并不同群组的消息。

在一些情况下，你可能需要单向消息转发，而不是双向转发。不用担心，CoolQ Telegram Bot X 贴心的考虑到了这一点，轻松胜任单向消息转发。

#### 智能转换

你可能已经发现，QQ 表情是一种特殊的文字。这难不倒 CoolQ Telegram Bot X，在转发到 Telegram 群组时，它会自动将 QQ 表情转换为对应的 emoji。在未来的计划中，我们将提供更多特殊消息内容的解析和智能转换。

## 安装

相当简单，仅需以下几步：

1. 安装酷 Q 主程序。

   Windows 用户请直接从 [酷 Q 官方网站](https://cqp.cc/)下载和安装酷 Q。

   Linux 用户推荐直接使用 Docker 镜像 [coolq/wine-coolq](https://hub.docker.com/r/coolq/wine-coolq/builds/)。

2. 从 [CoolQ Telegram Bot X 发布页](https://github.com/JogleLew/coolq-telegram-bot-x/releases) 下载 CoolQ Telegram Bot X 插件。

3. 将 cpk 文件放置到`酷Q根目录/app/` 中。

4. 启动酷 Q 主程序，启用插件。

如需更多信息，请访问本仓库 wiki 中的安装指导页面。

## 配置

一般情况下，如果你需要接收来自 Telegram 群组的消息，或发送消息到 Telegram 群组，那么一个**Telegram bot token** 需要正确填写到配置文件中。接下来在配置文件中自定义**转发规则**后，CoolQ Telegram Bot X 将按照你们配置工作。

如需更多信息，请访问本仓库 wiki 中的配置指导页面。

## 开发

源代码为 **Visual Studio 2017 项目**，需要支持 **C++ 17** 标准的编译器才能进行编译。请使用 **Microsoft/vcpkg** 安装以下依赖项：`boost`、`cpprestsdk`、`curl`、`nlohmann-json`、`openssl`、`libiconv`.

如需更多信息，请访问本仓库 wiki 中的开发指导页面。

如果您在使用或开发 CoolQ Telegram Bot X 中有任何的疑问，可以加入我们的 [Telegram 群](https://t.me/CoolqTelegramBot)进行讨论和交流。

## 替代品

在 C++ 版本问世之前，我们有个 Python 版本 [jqqqqqqqqqq/CoolQ Telegram Bot](https://github.com/jqqqqqqqqqq/coolq-telegram-bot)。

你可能会在这两个版本之间犹豫。那么应该选择使用哪个版本呢？简而言之，Python 版本更关注于新的功能和更强的可扩展性，而 C++ 版本聚焦于部署和使用上的简单和便捷。

|              | C++ 版本                                    | Python 版本    |
| ------------ | ------------------------------------------- | -------------- |
| 转发方向     | 支持单向转发，双向转发                      | 支持双向转发   |
| 转发群组     | QQ - Telegram, QQ - QQ, Telegram - Telegram | QQ - Telegram  |
| 文字转发     | 支持                                        | 支持           |
| 图片转发     | 不支持 (在计划中)                           | 支持           |
| 音频转发     | 不支持 (在计划中)                           | 部分支持       |
| 其它消息转发 | 转换成文字消息                              | 转换成文字消息 |

## 致谢

CoolQ Telegram Bot X 派生于 [jqqqqqqqqqq/CoolQ Telegram Bot](https://github.com/jqqqqqqqqqq/coolq-telegram-bot).

CoolQ Message SDK 基于 [CoolQ HTTP API](https://github.com/richardchien/coolq-http-api)

Telegram C++ SDK 使用了 [reo7sp/tgbot-cpp](https://github.com/reo7sp/tgbot-cpp) 的代码
