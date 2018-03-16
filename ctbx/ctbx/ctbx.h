#pragma once

#include "cqsdk/cqsdk.h"
#include "tgbot/tgbot.h"
#include "./constant.h"
#include "./utils/utils.h"

namespace ctbx::tgevent {
	void bot_receive_anymessage(const TgBot::Message::Ptr);
	void bot_handle_fowardmessage(const TgBot::Message::Ptr);
	void bot_handle_replymessage(const TgBot::Message::Ptr);
}

namespace ctbx::cqevent {
	void bot_on_enable();
	void bot_on_disable();
	void bot_on_exit();
	void bot_receive_groupmessage(const cq::event::GroupMessageEvent&);
}
