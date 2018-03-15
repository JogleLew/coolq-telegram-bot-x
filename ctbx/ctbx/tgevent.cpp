#include "./ctbx.h"

#include <thread>
#include <stdint.h>

namespace logging = cq::logging;

namespace ctbx::tgevent {
	void bot_receive_anymessage(const TgBot::Message::Ptr message) {
		if (message->chat->id != TG_GROUP)
			return;
		int64_t chat_id = message->chat->id;
		cq::Message msg = message->from->username + " : " + message->text;
		logging::info(u8"TGBot", u8"收到TG消息:" + message->text + "，开始转发");
		try {
			msg.send(cq::Target(BOT_QQ, QQ_GROUP, cq::Target::GROUP));
		}
		catch (const cq::exception::ApiError &e) {
			logging::debug(u8"TGBot", u8"Tg->QQ转发失败，错误码:" + std::to_string(e.code));
		}
	}
}