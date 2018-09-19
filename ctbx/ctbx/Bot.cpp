/// @file    Bot.cpp
/// @brief   CTBX Telgram Bot Part
/// @author  wtdcode
/// @date    2018-03-21
/// @note    Telegram Bot of CoolQ Telegram Bot X.
///

#include "./Bot.h"

namespace logging   = ctbx::logging; ///< Use thread-safe logging
namespace exception = cq::exception;
namespace api       = cq::api;
namespace dir       = cq::dir;

using namespace ctbx::cards;
using namespace ctbx::logging::bot;

namespace ctbx {
	typedef std::function<void(const boost::system::error_code&)> timer_callback;

	/// tg_msg_log
	/// @note   Log Telegram message
	/// @param  msg   Received Telegram message
	/// @param  group Group info
	/// @return void
	/// 
	void tg_msg_log(const TgBot::Message::Ptr& msg, const Group& group) {
		int64_t group_id = group.group_id;
		std::string group_type = (group.type == SOFTWARE_TYPE::QQ ? "QQ" : "TG");
		logging::info(INFO_RECV_MSG_FROM_TG_TAG, fmt::format(INFO_RECV_MSG_FROM_TG, msg->chat->id, msg->text, group_type, group_id));
		logging::debug(DEBUG_TG_MSG_TAG, fmt::format(DEBUG_TG_MSG_TEXT, msg->text != ""));
		logging::debug(DEBUG_TG_MSG_TAG, fmt::format(DEBUG_TG_MSG_IS_PHOTO, msg->photo.size()));
		logging::debug(DEBUG_TG_MSG_TAG, fmt::format(DEBUG_TG_MSG_IS_FORWARD, msg->forwardFrom.use_count()));
		if (msg->photo.size())
			for (auto &it : msg->photo)
				logging::debug(DEBUG_TG_MSG_TAG, fmt::format(DEBUG_TG_MSG_PHOTO, it->fileId));
		logging::debug(DEBUG_TG_MSG_TAG, fmt::format(DEBUG_TG_MSG_IS_STICKER, msg->sticker.use_count()));
		if (msg->sticker.use_count())
			logging::debug(DEBUG_TG_MSG_TAG, fmt::format(DEBUG_TG_MSG_STICKER, msg->sticker->fileId, msg->sticker->thumb->fileId));
		logging::debug(DEBUG_TG_MSG_TAG, fmt::format(DEBUG_TG_MSG_IS_AUDIO, msg->audio.use_count()));
		if (msg->audio.use_count())
			logging::debug(DEBUG_TG_MSG_TAG, fmt::format(DEBUG_TG_MSG_AUDIO, msg->audio->mimeType, msg->audio->performer, msg->audio->fileId));
	}

	/// cq_msg_log
	/// @note   Log CoolQ message
	/// @param  m     Received CoolQ message
	/// @param  group Group info
	/// @return void
	/// 
	void cq_msg_log(const cq::GroupMessageEvent& m, const Group& group) {
		cq::Message msg = m.message;
		int64_t group_id = group.group_id;
		std::string group_type = (group.type == SOFTWARE_TYPE::QQ ? "QQ" : "TG");
		logging::info(INFO_RECV_MSG_FROM_CQ_TAG,fmt::format(INFO_RECV_MSG_FROM_CQ, m.group_id, msg, group_type, group_id));
		logging::debug(DEBUG_CQ_MSG_TAG,fmt::format(DEBUG_CQ_MSG_RAW, msg));
		for (auto &it : msg) {
			logging::debug(DEBUG_CQ_MSG_TAG, fmt::format(DEBUG_CQ_MSG_TYPE, it.type));
			for(auto &d : it.data)
				logging::debug(DEBUG_CQ_MSG_TAG,fmt::format(DEBUG_CQ_MSG_DETAIL, d.first, d.second));
		}
	}

	/// constructor
	/// @note Constructor of Telegram bot
	/// 
	Bot::Bot() 
		: _config(ctbx::config::Config::get_config()), _tgbot(_config.get_bot_token()),  _polling(true) {
		
	}

	/// destructor
	/// @note Destructor of Telegram bot
	/// 
	Bot::~Bot() {
		logging::info(INFO_BOT_RUNNING_TAG, INFO_BOT_EXITING);
		_polling = false;
		if(_tgbot_thread.joinable())
			_tgbot_thread.join();
		Cards::stop_updating();
	}

	/// bot_on_enable
	/// @note   Initialize Telegram bot, execute when bot is enabled
	/// @return void
	/// 
	void Bot::bot_on_enable() {
		logging::info(INFO_BOT_RUNNING_TAG, INFO_BOT_INITIALIZING);
		logging::debug(DEBUG_BOT_RUNNING_TAG, DEBUG_BOT_STARTING);
		logging::debug(DEBUG_BOT_RUNNING_TAG, fmt::format(DEBUG_BOT_TOKEN, _config.get_bot_token()));
		std::string bot_name;
		try {
			bot_name = _tgbot.getApi().getMe()->username;
		}
		catch (const std::exception& e) {
			logging::debug(DEBUG_BOT_RUNNING_TAG, fmt::format(DEBUG_BOT_FAIL_TO_START, cq::utils::string_decode(std::string(e.what()), cq::utils::Encoding::ANSI)));
			logging::info(INFO_BOT_RUNNING_TAG, INFO_BOT_RETRY);
			cq::logging::info(CQ_LOG_TAG, INFO_BOT_RETRY);
			return;
		}
		_tgbot_set_events();
		Cards::start_updating(_config.get_all_from_groups());
		logging::debug(DEBUG_BOT_RUNNING_TAG, fmt::format(DEBUG_BOT_TOKEN, bot_name));
		_tgbot_start_polling();
		logging::info(INFO_BOT_RUNNING_TAG, INFO_BOT_START_SUCCESSFULLY);
	}

	/// qq_receive_groupmessage
	/// @note   Forward message when CoolQ message received 
	/// @param  e Group message from CoolQ
	/// @return void
	/// 
	void Bot::qq_receive_groupmessage(const cq::event::GroupMessageEvent & e){
		for (auto &it : _config.get_forward_groups({ SOFTWARE_TYPE::QQ, e.group_id })) {
			ctbx::message::UnifiedMessage msg(e);
			cq_msg_log(e, it);
			msg.send(it, _tgbot, ctbx::types::SOFTWARE_TYPE::QQ);
		}
	}

	/// _tg_receive_groupmessage
	/// @note   Forward message when Telegram message received 
	/// @param  tgmsg Group message from Telegram
	/// @return void
	/// 
	void Bot::_tg_receive_groupmessage(const TgBot::Message::Ptr& tgmsg) {
		for (auto &it : _config.get_forward_groups({ SOFTWARE_TYPE::TG, tgmsg->chat->id })) {
			ctbx::message::UnifiedMessage msg(tgmsg, _tgbot);
			tg_msg_log(tgmsg, it);
			msg.send(it, _tgbot, ctbx::types::SOFTWARE_TYPE::TG);
		}
	}

	/// _tgbot_set_events
	/// @note   Set up callback on any message for Telegram bot
	/// @return void
	/// 
	void Bot::_tgbot_set_events(){
		_tgbot.getEvents().onAnyMessage([this](const auto& msg) {_tg_receive_groupmessage(msg); });
	}

	/// _tgbot_start_polling
	/// @note   Start polling process of Telegram bot
	/// @return void
	/// 
	void Bot::_tgbot_start_polling() {
		_tgbot_thread =  std::move(std::thread([this]() {
			TgBot::TgLongPoll longpoll(_tgbot, 100, 10);
			while (_polling) {
				try {
					while (_polling)
						longpoll.start();
				}
				catch (const std::exception& e) {
					logging::error(ERROR_BOT_RUNNING_TAG, fmt::format(ERROR_LONG_POLL, cq::utils::string_decode(std::string(e.what()), cq::utils::Encoding::ANSI)));
				}
			}
		}));
	}
}