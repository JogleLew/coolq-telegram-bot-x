//
// Created by Jogle Lew on 2018/2/11.
//

#include "app.h"
#include "tgsdk.h"
#include "tgbot\tgbot.h"
#include "forward\traffic_center.h"
#include <thread>

using namespace std;
using namespace TgBot;

thread tgThread;
int isRunning = 0;

void init_tgsdk(string token) {
	try {
		if (isRunning == 1)
			return;
		tgThread = thread(start_bot, token);
		isRunning = 1;
	}
	catch (exception& e) {
		Log::e(u8"Telegram", e.what());
	}
}

void recycle_tgsdk() {
	try {
		tgThread.~thread();
	}
	catch (exception& e) {
		Log::e(u8"Telegram", e.what());
	}
}

void start_bot(string token) {
	try {
		TgBot::Bot bot(token);
		tgbot = make_shared<TgBot::Bot>(bot);
		bot.getEvents().onCommand("start", [&bot](TgBot::Message::Ptr message) {
			bot.getApi().sendMessage(message->chat->id, "Coolq Telegram Bot X \nhttps://github.com/JogleLew/coolq-telegram-bot-x");
		});
		bot.getEvents().onCommand("showid", [&bot](TgBot::Message::Ptr message) {
			bot.getApi().sendMessage(message->chat->id, std::to_string(message->chat->id));
		});
		bot.getEvents().onAnyMessage([&bot](TgBot::Message::Ptr message) {
			// printf("User wrote %s\n", message->text.c_str());
			if (StringTools::startsWith(message->text, "/start") || 
				StringTools::startsWith(message->text, "/showid")) {
				return;
			}
			
			string log_str = u8"群: " + to_string(message->chat->id) + 
			u8" 用户: " + message->from->username + u8"(" + to_string(message->from->id) + u8")" + 
			u8" 消息内容: " + message->text;
			Log::i(u8"[↓]tg消息", log_str);
			tg_any_message(message);
		});
		// printf("Bot username: %s\n", bot.getApi().getMe()->username.c_str());
		Log::i(u8"Telegram", u8"Bot" + bot.getApi().getMe()->username + u8"已初始化");
		TgBot::TgLongPoll longPoll(bot);
		while (true) {
			//printf("Long poll started\n");
			Log::d(u8"Telegram", u8"正在进行轮询");
			longPoll.start();
		}
	}
	catch (TgBot::TgException& e) {
		Log::e(u8"Telegram", e.what());
	}
}