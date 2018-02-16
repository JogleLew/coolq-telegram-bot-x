#pragma once

#include "app.h"
#include "tgbot\tgbot.h"

class UnifiedMessage {
public:
	/* 1 for QQ Message, 2 for Telegram Message */
	int32_t message_source = 0;
	/* 1 for text, 2 for sticker, 3 for photo, 4 for document, 5 for audio, 6 for video, 7 for voice */
	int32_t message_type = 0;
	int64_t from_group_id = 0;
	int64_t from_user_id = 0;
	int32_t is_forward = 0;
	int64_t forward_from_group_id = 0;
	std::string forward_from_group_name = "";
	int64_t forward_from_user_id = 0;
	std::string forward_from_user_name = "";
	int32_t is_reply = 0;
	int64_t reply_user_id = 0;
	std::string reply_user_name = "";
	std::string from_user_name = "";
	std::string message_content = "";
	std::string placeholder = "";
	std::string caption = "";

	void fillTelegramMessage(TgBot::Message::Ptr message);
	void fillQQGroupMessage(int32_t sub_type, int32_t msg_id, int64_t from_group, int64_t from_qq, std::string from_anonymous, std::string msg, int32_t font);
	std::string parseTextMessage();
	void sendToTelegramGroup(int64_t tg_id);
	void sendToQQGroup(int64_t qq_id);
	void sendToGroup(int32_t group_type, int64_t group_id);
};