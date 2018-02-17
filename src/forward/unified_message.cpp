#include "unified_message.h"
#include "structs.h"
#include "api\api.h"

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
	
	message_type = 1;
	message_content = message->text;
	if (message->sticker) {
		message_type = 2;
		message_content = message->sticker->fileId;
		placeholder = "[" + message->sticker->emoji + " sticker]";
	}
	else if (message->photo.size() > 0) {
		message_type = 3;
		message_content = message->photo.at(message->photo.size() - 1)->fileId;
		caption = message->caption;
		placeholder = u8"[图片]" + message->caption;
	}
	else if (message->document) {
		message_type = 4;
		message_content = message->document->fileId;
		caption = message->caption;
		placeholder = u8"[文件]" + message->caption;
	}
	else if (message->audio) {
		message_type = 5;
		message_content = message->audio->fileId;
		placeholder = u8"[音频]";
	}
	else if (message->video) {
		message_type = 6;
		message_content = message->video->fileId;
		caption = message->caption;
		placeholder = u8"[视频]" + message->caption;
	}
	else if (message->voice) {
		message_type = 7;
		message_content = message->voice->file_id;
		placeholder = u8"[语音]";
	}
	else if (message->contact) {
		message_type = 1;
		message_content = u8"通讯录共享\r\n姓名: " + message->contact->firstName + " " + message->contact->lastName;
		message_content += u8"\r\n手机号: " + message->contact->phoneNumber;
	}
	else if (message->location) {
		message_type = 1;
		message_content = u8"位置共享";
		message_content += u8"\r\n经度: " + std::to_string(message->location->longitude);
		message_content += u8"\r\n纬度: " + std::to_string(message->location->latitude);
	}
	else if (message->newChatMember) {
		message_type = 1;
		message_content = u8"新用户: " + message->newChatMember->firstName + " " + message->newChatMember->lastName;
	}
	else if (message->leftChatMember) {
		message_type = 1;
		message_content = u8"退群用户: " + message->leftChatMember->firstName + " " + message->leftChatMember->lastName;
	}
	else if (message->newChatTitle.length() > 0) {
		message_type = 1;
		message_content = u8"修改群名为: " + message->newChatTitle;
	}
}

void UnifiedMessage::fillQQGroupMessage(const json& payload) {
	message_source = 1;
	message_type = 1;
	if (payload.find("group_id") != payload.end())
		from_group_id = payload["group_id"];
	else
		from_group_id = payload["user_id"];
	from_user_id = payload["user_id"];

	// get group member
	from_user_name = std::to_string(from_user_id);
	if (payload.find("group_id") != payload.end()) {
		auto bytes = sdk->get_group_member_info_raw(payload["group_id"], payload["user_id"], false);
		if (bytes.size() >= GroupMember::MIN_SIZE) {
			auto member = GroupMember::from_bytes(bytes);
			from_user_name = member.card;
		}
	}

	if (payload.find("anonymous") != payload.end()) {
		std::string anonymous_name = payload["anonymous"];
		if (anonymous_name.length() > 0)
			from_user_name = u8"[匿名用户]" + anonymous_name;
	}

	// fill message content
	std::string str = payload["message"];
	message_content = str;
}

std::string UnifiedMessage::parseTextMessage() {
	std::string msg_text = "";
	if (message_type == 1)
		msg_text = from_user_name + u8": " + message_content;
	else
		msg_text = from_user_name + u8": " + placeholder;
	if (is_forward)
		msg_text += u8"\n[转发自: " + forward_from_user_name + forward_from_group_name + u8"]";
	if (is_reply)
		msg_text += u8"\n[回复: " + reply_user_name + u8"]";
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