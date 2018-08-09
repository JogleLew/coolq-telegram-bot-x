/// @file    Bot.cpp
/// @brief   CTBX Telgram Bot Part
/// @author  wtdcode
/// @date    2018-03-21
/// @note    Telegram Bot of CoolQ Telegram Bot X.
///

#include "./Bot.h"
#include "Bot.h"

namespace logging   = ctbx::logging; ///< Use thread-safe logging
namespace exception = cq::exception;
namespace api       = cq::api;
namespace dir       = cq::dir;

using namespace ctbx::cards;

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
		logging::info(u8"Forward", u8"从TG群" + std::to_string(msg->chat->id) + 
			"收到消息 : " + msg->text + "，开始转发到" + group_type + "群" + std::to_string(group_id));
		logging::debug(u8"MsgDebugInfo", u8"文本:" + std::to_string(msg->text != ""));
		logging::debug(u8"MsgDebugInfo", u8"照片:" + std::to_string(msg->photo.size()));
		logging::debug(u8"MsgDebugInfo", u8"Forward:" + std::to_string(msg->forwardFrom.use_count()));
		if (msg->photo.size())
			for (auto &it : msg->photo)
				logging::debug(u8"MsgDebugInf", u8"照片ID:" + it->fileId);
		logging::debug(u8"MsgDebugInfo", u8"Sticker:" + std::to_string(msg->sticker.use_count()));
		if (msg->sticker.use_count())
			logging::debug(u8"MsgDebugInfo", u8"StickerID:" + msg->sticker->fileId + " thumb:" + msg->sticker->thumb->fileId);
		logging::debug(u8"MsgDebugInfo", u8"音频:" + std::to_string(msg->audio.use_count()));
		if (msg->audio.use_count())
			logging::debug(u8"MsgDebugInfo", u8"Type:" + msg->audio->mimeType + " performer:" + msg->audio->performer + " field:" + msg->audio->fileId);
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
		logging::info(u8"MsgDebugInfo", u8"从QQ群" + std::to_string(m.group_id) +
			"收到消息 : " + msg + "，开始转发到" + group_type + "群" + std::to_string(group_id));
		logging::debug(u8"MsgDebugInfo", u8"raw:" + std::to_string(msg));
		for (auto &it : msg) {
			logging::debug(u8"MsgDebugInfo", "Type:" + it.type);
			for(auto &d : it.data)
				logging::debug(u8"MsgDebugInfo", "first:" + d.first + "second:" + d.second);
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
		logging::info(u8"Bot", u8"Bot即将退出");
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
		logging::info(u8"Bot", u8"Bot开始初始化");
		logging::debug(u8"TGBot", u8"开始初始化TGBot，根据网络情况需要的时间有较大区别");
		logging::debug(u8"TGBot", u8"TGBot的Token为:" + std::to_string(_config.get_bot_token()));
		std::string bot_name;
		try {
			bot_name = _tgbot.getApi().getMe()->username;
		}
		catch (const TgBot::TgException& e) {
			logging::debug(u8"TGBot", u8"初始化失败，原因：" + cq::utils::string_decode(std::string(e.what()), cq::utils::Encoding::ANSI));
			logging::info(u8"TGBot", u8"初始化失败，请尝试重新启动Bot。");
			cq::logging::info(u8"TGBot", u8"初始化失败，请尝试重新启动Bot。");
			return;
		}
		catch (const std::exception& e) {
			logging::debug(u8"TGBot", u8"初始化失败，原因：" + cq::utils::string_decode(std::string(e.what()), cq::utils::Encoding::ANSI));
			logging::info(u8"TGBot", u8"初始化失败，请尝试重新启动Bot。");
			cq::logging::warning(u8"TGBot", u8"初始化失败，请尝试重新启动Bot。");
			return;
		}
		catch (...) {
			logging::info(u8"TGBot", u8"初始化失败，请尝试重新启动Bot。");
			cq::logging::info(u8"TGBot", u8"初始化失败，请尝试重新启动Bot。");
			return;
		}
		_tgbot_set_events();
		Cards::start_updating(_config.get_all_from_groups());
		logging::debug(u8"TGBot", u8"TGBot初始化完成，TGBot用户名:" + bot_name);
		_tgbot_start_polling();
		logging::info(u8"Bot", u8"初始化完毕");
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
			try {
				while(_polling)
					longpoll.start();
			}
			catch (const TgBot::TgException& e) {
				logging::error(u8"Bot", "LongPoll错误，原因：" + std::string(e.what()));
			}
			catch (const std::exception& e) {
				logging::error(u8"Bot", "LongPoll错误，原因：" + std::string(e.what()));
			}
		}));
	}
}