# CoolQ Telegram Bot X

Language: 🇺🇸 English | 🇨🇳 [简体中文](https://github.com/JogleLew/coolq-telegram-bot-x/blob/master/README-CN.md)

<img src="https://raw.githubusercontent.com/JogleLew/coolq-telegram-bot-x/resource/pic/logo.png" width="200">

CoolQ Telegram Bot X is a message forwarding bot. This bot can help to forward messages from one [QQ](https://im.qq.com) / [Telegram](https://telegram.org/) group to another. With simple config, you can customize the forwarding logic in a convenient way. Feel free to link between QQ and Telegram, or merge different messages to one place.

## Features

#### Deep Integration

CoolQ Telegram Bot X is an integration of QQ Bot and Telegram Bot. **[CoolQ](https://cqp.cc/) Message SDK** ensures the stability and availability of receiving and sending QQ messages every time. **Telegram C++ SDK** based on official [Telegram Bot API](https://core.telegram.org/bots/api) shows its power to handle different kinds of messages. Forwarding logic unit will parse and transfer messages which QQ Bot or Telegram Bot receive to another group according to forward config.

All message SDKs and forwarding logic unit are packed into a **CoolQ plugin**. Start CoolQ, enable the plugin, and enjoy :)

<img src="https://raw.githubusercontent.com/JogleLew/coolq-telegram-bot-x/resource/pic/readme/readme-1.png" width="300">

#### Multi-direction Forwarding

CoolQ Telegram Bot X supports various forwarding type. No matter **QQ to Telegram**, or **Telegram to QQ**, it can parse and transfer messages correctly. Besides, **QQ to QQ** and **Telegram to Telegram** message forwarding can help you organize and merge messages from different groups.

In some cases, you may need one-direction forwarding rather than bi-direction forwarding. Don't worry, CoolQ Telegram Bot X is still capable to do it.

<img src="https://raw.githubusercontent.com/JogleLew/coolq-telegram-bot-x/resource/pic/readme/readme-2.png" width="300">

#### Smart Message Conversion

You may notice that QQ faces are special characters. CoolQ Telegram Bot X is so smart, converting QQ faces to emoji automatically when sending to Telegram group. In future work, we will support more special message conversion.

<img src="https://raw.githubusercontent.com/JogleLew/coolq-telegram-bot-x/resource/pic/readme/readme-3.png" width="300">

## Installation

Quite easy, just follow these steps:

1. Install CoolQ Main Program.

   For Windows users, please download CoolQ from [official website](https://cqp.cc/).

   For Linux users, it is recommended to deploy Docker image from [coolq/wine-coolq](https://hub.docker.com/r/coolq/wine-coolq/builds/).

2. Download CoolQ plugin from [relesase page](https://github.com/JogleLew/coolq-telegram-bot-x/releases).

3. Place the cpk file to `CoolQ_Root_Directory/app/` .

4. Start CoolQ Main Program, enable the plugin.

For more details, please visit the [installation guide in repository wiki](https://github.com/JogleLew/coolq-telegram-bot-x/wiki/%E5%AE%89%E8%A3%85%E6%8C%87%E5%AF%BC).

## Configuration

Generally, if you need message sending to / receiving from Telegram groups, a **Telegram bot token** is needed to fill in config file. Then customize the **forwarding rule** in config file, you will find the bot works functionally.

For more details, please visit the [configuration guide in repository wiki](https://github.com/JogleLew/coolq-telegram-bot-x/wiki/%E9%85%8D%E7%BD%AE%E6%8C%87%E5%AF%BC).

## Join Development

The source code is a **[Visual Studio 2017](https://www.visualstudio.com) project**, and requires a compiler supporting **C++ 17** standard. Please use **[Microsoft/vcpkg](https://github.com/Microsoft/vcpkg)** to install dependencies: `boost`、`cpprestsdk`、`curl`、`nlohmann-json`、`openssl`、`libiconv`.

For more details, please visit the [development guide in repository wiki](https://github.com/JogleLew/coolq-telegram-bot-x/wiki/%E5%BC%80%E5%8F%91%E6%8C%87%E5%AF%BC).

If you find some trouble in using or developing CoolQ Telegram Bot X, you can join our [Support Group (Telegram)](https://t.me/CoolqTelegramBot).

## Alternative

You know, There is a Python version:  [jqqqqqqqqqq/CoolQ Telegram Bot](https://github.com/jqqqqqqqqqq/coolq-telegram-bot).

What to choose? In short, the Python version focuses more on new features and extensiblility, but the C++ version cares more on convenience and simple deployment.

|                          | C++ Version                                       | Python Version          |
| ------------------------ | ------------------------------------------------- | ----------------------- |
| Forwarding direction     | One-direction forwarding, bi-direction forwarding | Bi-direction forwarding |
| Forwarding group         | QQ - Telegram, QQ - QQ, Telegram - Telegram       | QQ - Telegram           |
| Text forwarding          | Support                                           | Support                 |
| Picture forwarding       | Unsupport (in plan)                               | Support                 |
| Audio forwarding         | Unsupport (in plan)                               | Partly support          |
| Other message forwarding | Change to text message                            | Change to text message  |

## Credit

CoolQ Telegram Bot X is derived from [jqqqqqqqqqq/CoolQ Telegram Bot](https://github.com/jqqqqqqqqqq/coolq-telegram-bot).

CoolQ Message SDK is based on [richardchien/coolq-http-api](https://github.com/richardchien/coolq-http-api)

Telegram C++ SDK uses the code from [reo7sp/tgbot-cpp](https://github.com/reo7sp/tgbot-cpp)

