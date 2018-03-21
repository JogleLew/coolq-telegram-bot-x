#pragma once

#include "ctbx/stdafx.h"
#include "ctbx/types/Types.h"
#include "ctbx/logging/Logging.h"
#include "ctbx/cards/Cards.h"

namespace ctbx::message {
	class UnifiedMessage {
	private:
		std::string _plain_text;
		std::shared_ptr<ctbx::types::User> _from;
		std::shared_ptr<ctbx::types::User> _reply_to;
		std::shared_ptr<ctbx::types::User> _forward_from;
		std::shared_ptr<ctbx::message::UnifiedMessage> _replied_message;
		std::shared_ptr<ctbx::types::Photo> _photo; // unimplemented
	public:
		explicit UnifiedMessage(const cq::GroupMessageEvent&);
		explicit UnifiedMessage(const TgBot::Message::Ptr&);
		void send(const ctbx::types::Group&, const TgBot::Bot&);
		void send_to_qq(const int64_t);
		void send_to_tg(const int64_t, const TgBot::Bot&);
	private:
		std::string _parse_text();
	};
}