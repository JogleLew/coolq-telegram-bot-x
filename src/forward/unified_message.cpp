#include "unified_message.h"

void UnifiedMessage::fillTelegramMessage(TgBot::Message::Ptr message) {
	message_source = 2;
	from_group_id = message->chat->id;
	if (message->from) {
		from_user_id = message->from->id;
		from_user_name = message->from->firstName + " " + message->from->lastName;
	}
	else {
		from_user_id = 0;
		from_user_name = u8"系统消息";
	}
	Log::d(u8"Debug", u8"1");
	if (message->forwardFrom) {
		is_forward = 1;
		if (message->forwardFrom) {
			forward_from_user_id = message->forwardFrom->id;
			forward_from_user_name = message->forwardFrom->firstName + " " + message->forwardFrom->lastName;
		}
		if (message->forwardFromChat) {
			forward_from_group_id = message->forwardFromChat->id;
			forward_from_group_name = message->forwardFromChat->title;
		}
	}
	Log::d(u8"Debug", u8"2");
	if (message->replyToMessage) {
		is_reply = 1;
		if (message->replyToMessage->from) {
			reply_user_id = message->replyToMessage->from->id;
			reply_user_name = message->replyToMessage->from->firstName + " " + message->replyToMessage->from->lastName;
		}
		else {
			reply_user_id = 0;
			reply_user_name = u8"系统消息";
		}
	}
	Log::d(u8"Debug", u8"3");
	message_type = 1;
	message_content = message->text;
	if (message->sticker) {
		message_type = 2;
		message_content = message->sticker->fileId;
		placeholder = "[" + message->sticker->emoji + " sticker]";
		Log::d(u8"Debug", u8"4");
	}
	else if (message->photo.size() > 0) {
		message_type = 3;
		message_content = message->photo.at(message->photo.size() - 1)->fileId;
		caption = message->caption;
		placeholder = u8"[图片]" + message->caption;
		Log::d(u8"Debug", u8"5");
	}
	else if (message->document) {
		message_type = 4;
		message_content = message->document->fileId;
		caption = message->caption;
		placeholder = u8"[文件]" + message->caption;
		Log::d(u8"Debug", u8"6");
	}
	else if (message->audio) {
		message_type = 5;
		message_content = message->audio->fileId;
		placeholder = u8"[音频]";
		Log::d(u8"Debug", u8"7");
	}
	else if (message->video) {
		message_type = 6;
		message_content = message->video->fileId;
		caption = message->caption;
		placeholder = u8"[视频]" + message->caption;
		Log::d(u8"Debug", u8"8");
	}
	else if (message->voice) {
		message_type = 7;
		message_content = message->voice->file_id;
		placeholder = u8"[语音]";
		Log::d(u8"Debug", u8"9");
	}
	else if (message->contact) {
		message_type = 1;
		message_content = u8"通讯录共享\r\n姓名: " + message->contact->firstName + " " + message->contact->lastName;
		message_content += u8"\r\n手机号: " + message->contact->phoneNumber;
		Log::d(u8"Debug", u8"10");
	}
	else if (message->location) {
		message_type = 1;
		message_content = u8"位置共享";
		message_content += u8"\r\n经度: " + std::to_string(message->location->longitude);
		message_content += u8"\r\n纬度: " + std::to_string(message->location->latitude);
		Log::d(u8"Debug", u8"11");
	}
	else if (message->newChatMember) {
		message_type = 1;
		message_content = u8"新用户: " + message->newChatMember->firstName + " " + message->newChatMember->lastName;
		Log::d(u8"Debug", u8"12");
	}
	else if (message->leftChatMember) {
		message_type = 1;
		message_content = u8"退群用户: " + message->leftChatMember->firstName + " " + message->leftChatMember->lastName;
		Log::d(u8"Debug", u8"13");
	}
	else if (message->newChatTitle.length() > 0) {
		message_type = 1;
		message_content = u8"修改群名为: " + message->newChatTitle;
		Log::d(u8"Debug", u8"14");
	}
}

void UnifiedMessage::fillQQGroupMessage(int32_t sub_type, int32_t msg_id, int64_t from_group, int64_t from_qq, std::string from_anonymous, std::string msg, int32_t font) {
	message_source = 1;
	message_type = 1;
	from_group_id = from_group;
	from_user_id = from_qq;
	from_user_name = std::to_string(from_qq);
	if (from_anonymous.length() > 0)
		from_user_name = u8"[匿名用户]" + from_anonymous;
	message_content = msg;
}

std::string UnifiedMessage::parseTextMessage() {
	std::string msg_text = "";
	if (message_type == 1)
		msg_text = from_user_name + u8": " + message_content;
	else
		msg_text = from_user_name + u8": " + placeholder;
	if (is_forward)
		msg_text += u8"\n转发自: " + forward_from_user_name + forward_from_group_name;
	if (is_reply)
		msg_text += u8"\n回复: " + reply_user_name;
	return msg_text;
}

void UnifiedMessage::sendToTelegramGroup(int64_t tg_id) {
	if (!tgbot)
		return;
	tgbot->getApi().sendMessage(tg_id, parseTextMessage());
}

void UnifiedMessage::sendToQQGroup(int64_t qq_id) {
	sdk->send_group_msg(qq_id, parseTextMessage());
}

void UnifiedMessage::sendToGroup(int32_t group_type, int64_t group_id) {
	if (group_type == 1)
		sendToQQGroup(group_id);
	else if (group_type == 2)
		sendToTelegramGroup(group_id);
}