#pragma once

#include "ctbx/stdafx.h"
#include "ctbx/logging/Logging.h"
#include "ctbx/cards/Cards.h"
#include "ctbx/config/Config.h"
#include "ctbx/types/Types.h"
#include "ctbx/unifiedmessage/UnifiedMessage.h"
#include "boost/asio.hpp"

namespace ctbx {
	using ctbx::types::GROUP_TYPE;
	using ctbx::types::Group;
	class Bot {
	private:
		ctbx::config::Config _config;
		TgBot::Bot _tgbot;
		std::thread _tgbot_thread;
		bool _polling;
	public:
		explicit Bot(const std::string&, int32_t=3);
		~Bot();
	public:
		void bot_on_enable();
		void qq_receive_groupmessage(const cq::event::GroupMessageEvent&);
		bool config_valid() const;
	private:
		void _tg_receive_groupmessage(const TgBot::Message::Ptr&);
		void _tgbot_set_events();
		void _tgbot_start_polling();
	};
}