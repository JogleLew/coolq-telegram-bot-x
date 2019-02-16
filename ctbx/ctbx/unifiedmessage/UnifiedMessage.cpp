/// @file    UnifedMessage.cpp
/// @brief   CTBX Unified Message
/// @author  wtdcode
/// @date    2018-03-21
/// @note    Unified message management: parse and send
///

#include "./UnifiedMessage.h"

#include <locale>
#include <codecvt>

namespace types   = ctbx::types;
namespace logging = ctbx::logging;
namespace type    = ctbx::types;
namespace image   = ctbx::image;

using namespace ctbx::cards;
using namespace ctbx::logging::UnifiedMessage;

namespace ctbx::message {



	/// constructor (from CoolQ message)
	/// @note  Constructor of unified message
	/// @param cqgmsg CoolQ message
	/// 
	UnifiedMessage::UnifiedMessage(const cq::GroupMessageEvent& cqgmsg)
		: _unified_card(""), _image_count(0), _image_only(false), _from({ types::SOFTWARE_TYPE::QQ, cqgmsg.group_id, Cards::get_card(cqgmsg.group_id, cqgmsg.user_id) }) {
		int64_t from_group_id = cqgmsg.group_id;
		int64_t from_user_id = cqgmsg.user_id;
		cq::Message msg_list(cqgmsg.message);
		for (auto it = msg_list.begin(); it != msg_list.end(); it++) {
			if (it->type == "at") {
				int64_t reply_qq = std::stoll(it->data.at("qq"));
				_segs.emplace_back(
					new UReply({
						types::SOFTWARE_TYPE::QQ,
						reply_qq,
						Cards::get_card(from_group_id, reply_qq)
						}));
				msg_list.erase(it);
				break;
			}
		}
		_image_only = true;
		for (auto&it : msg_list)
			if (it.type != "image") {
				_image_only = false;
				break;
			}
		for (auto it = msg_list.begin(); it != msg_list.end();) {
			if (it->type == "image") {
				_segs.emplace_back(new UImage(it->data.at("file")));
				if (!_image_only) {
					it->type = "text";
					_image_count++;
					it->data["text"] = "[图片" + std::to_string(_image_count) + "]";
				} else {
					it = msg_list.erase(it);
					_image_count++;
				}
			}
			else
				it++;
		}
		std::string plain_text = "";
		for (auto it = msg_list.begin(); it != msg_list.end();) {
			if (it->type == "qq") {
				plain_text += "@";
				plain_text += Cards::get_card(from_group_id, std::stoll(it->data.at("qq")));
				it = msg_list.erase(it);
			}
			else if (it->type == "text") {
				plain_text += it->data.at("text");
				it = msg_list.erase(it);
			}
			else
				it++;
		}
		_segs.emplace_back(new UPlain(plain_text));
		_debug_remaining_msg(msg_list);
		_debug_all_segs();
	}

	/// constructor (from Telegram message)
	/// @note  Constructor of unified message
	/// @param tgmsg Telegram message
	/// @param tgbot Telegram bot
	/// 
	UnifiedMessage::UnifiedMessage(const TgBot::Message::Ptr& tgmsg, const TgBot::Bot& tgbot)
		: _unified_card(""), _image_count(0), _image_only(false), _from({ types::SOFTWARE_TYPE::TG,tgmsg->from->id,tgmsg->from->firstName }) {
		if (tgmsg->forwardFrom.use_count()) {
			_segs.emplace_back(new UForward({ types::SOFTWARE_TYPE::TG,tgmsg->forwardFrom->id,tgmsg->forwardFrom->firstName }));
		}
		if (tgmsg->replyToMessage.use_count()) {
			_segs.emplace_back(new UReply({ types::SOFTWARE_TYPE::TG, tgmsg->replyToMessage->from->id, tgmsg->replyToMessage->from->firstName }));
		}
		if (tgmsg->sticker.use_count()) {
			_image_count++;
			_segs.emplace_back(new UImage(tgmsg->sticker, tgbot));
		}
		if (!tgmsg->photo.empty()) {
			_image_count++;
			_segs.emplace_back(new UImage(*tgmsg->photo.rbegin(), tgbot));
		}
		if (!tgmsg->caption.empty())
			_segs.emplace_back(new UPlain(tgmsg->caption));
		if (tgmsg->text != "") {
			_segs.emplace_back(new UPlain(_preprocess_rich_text(tgmsg)));
		}
	}

	/// send
	/// @note   Send unified message to certain group
	/// @param  group Telegram group
	/// @param  bot   Telegram bot
	/// @param  from  Where the message is from
	/// @return void 
	/// 
	void UnifiedMessage::send(const ctbx::types::Group& group, const TgBot::Bot& bot, ctbx::types::SOFTWARE_TYPE from) {
		if (group.type == types::SOFTWARE_TYPE::QQ)
			send_to_qq(group.group_id, bot, from);
		else
			send_to_tg(group.group_id, bot, from);
	}

	/// send_to_qq
	/// @note   Send unified message to certain QQ group
	/// @param  group_id QQ group id
	/// @param  bot      Telegram bot
	/// @param  from     Where the message is from
	/// @return void 
	/// 
	void UnifiedMessage::send_to_qq(const int64_t group_id, const TgBot::Bot& bot, ctbx::types::SOFTWARE_TYPE from) {
		if (_unified_card.empty())
			_unified_card = _parse_card();
		short image_index = 1;
		for (auto& it : _segs) {
			if (it.type == MSG_TYPE::Image) {
				if(!it.image->send_to_qq(group_id, bot, _unified_card + "(" + std::to_string(image_index) + "/" + std::to_string(_image_count) + ")", from)){
					cq::api::send_group_msg(group_id, "[图片" +  std::to_string(image_index) + "拉取失败]");
				}
				image_index++;
			}
		}
		if (!_image_only) {
			for (auto& it : _segs) {
				if (it.type == MSG_TYPE::Plain) {
					std::string text = _unified_card + *(it.plain);
					cq::Message msg = text;
					try {
						msg.send(cq::Target(group_id, cq::Target::Type::GROUP));
					}
					catch (const cq::exception::ApiError& e) {
						logging::error(ERROR_UNFMSG_TAG, fmt::format(ERROR_CQ_SEND_MSG, text, std::string(e.what())));
					}
				}
			}
		}
	}

	/// send_to_tg
	/// @note   Send unified message to certain Telegram group
	/// @param  group_id Telegram group id
	/// @param  bot      Telegram bot
	/// @param  from     Where the message is from
	/// @return void 
	/// 
	void UnifiedMessage::send_to_tg(const int64_t group_id, const TgBot::Bot & bot, ctbx::types::SOFTWARE_TYPE from) {
		if (_unified_card.empty())
			_unified_card = _parse_card();
		short image_index = 1;
		for (auto& it : _segs) {
			if (it.type == MSG_TYPE::Image) {
				std::string cap = _unified_card + "(" + std::to_string(image_index) + "/" + std::to_string(_image_count) + ")";
				if (!it.image->send_to_tg(group_id, bot, cap, from)) {
					bot.getApi().sendMessage(group_id, "[图片" + std::to_string(image_index) + "拉取失败]");
				}
				image_index++;
			}
		}
		if (!_image_only) {
			for (auto& it : _segs) {
				if (it.type == MSG_TYPE::Plain) {
					std::string text = _unified_card + *(it.plain);
					try {
						bot.getApi().sendMessage(group_id, text);
					}
					catch (const TgBot::TgException::exception& e) {
						logging::error(ERROR_UNFMSG_TAG, fmt::format(ERROR_CQ_SEND_MSG, text, std::string(e.what())));
					}
				}
			}
		}
	}

	/// _parse_card
	/// @note   Parse member card accroding to unified message
	/// @return Member card name
	/// 
	std::string UnifiedMessage::_parse_card() {
		std::string text = _from.card;
		for (auto it = _segs.begin(); it != _segs.end(); it++) {
			if (it->type == MSG_TYPE::Reply) {
				text += "(Reply to " + it->reply->card + ")";
				_segs.erase(it);
				break;
			}
			else if (it->type == MSG_TYPE::Forward) {
				text += "(Forward from " + it->forward->card + ")";
				_segs.erase(it);
				break;
			}
			else
				break;
		}
		text += ": ";
		return text;
	}

	std::string UnifiedMessage::_preprocess_rich_text(const TgBot::Message::Ptr& tgmsg) {
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		std::wstring text = converter.from_bytes(tgmsg->text);
		std::string result = tgmsg->text;
		for (auto& entity : tgmsg->entities) {
			if (entity->type == "text_link") {
				result = std::move(
					fmt::format("{}[{}]({}){}", 
					converter.to_bytes(text.substr(0, entity->offset)), 
					converter.to_bytes(text.substr(entity->offset, entity->length)), 
					entity->url, 
					converter.to_bytes(text.substr(entity->offset + entity->length)))
				);
			}
			// ok, we dont't implement other types now. :)
		}
		return result;
	}

	/// _debug_remaining_msg
	/// @note   Logging status of remaining message
	/// @param  msg  Remaining message
	/// @return void
	/// 
	void UnifiedMessage::_debug_remaining_msg(const cq::Message& msg) {
		logging::debug(DEBUG_UNFMSG_TAG, DEBUG_UNFMSG_CQ_MSG);
		if (msg.empty())
			logging::debug(DEBUG_UNFMSG_TAG, DEBUG_UNFMSG_CQ_MSG_EMPTY);
		else
			logging::debug(DEBUG_UNFMSG_TAG, DEBUG_UNFMSG_CQ_MSG_NOT_EMPTY);
		for (auto& it : msg) {
			logging::debug(DEBUG_UNFMSG_TAG, fmt::format(DEBUG_UNFMSG_CQ_MSG_LEFT_DETAIL, it.type, it.data.begin()->second));
		}
	}

	/// _debug_all_segs
	/// @note   Logging info of unified message
	/// @return void
	/// 
	void UnifiedMessage::_debug_all_segs() {
		logging::debug(DEBUG_UNFMSG_TAG, DEBUG_UNFMSG_MSG);
		for (auto& it : _segs) {
			switch (it.type) {
			case MSG_TYPE::Forward:
				logging::debug(DEBUG_UNFMSG_TAG, fmt::format(DEBUG_UNFMSG_FORWARD, it.forward->card));
				break;
			case MSG_TYPE::Reply:
				logging::debug(DEBUG_UNFMSG_TAG, fmt::format(DEBUG_UNFMSG_REPLY, it.reply->card));
				break;
			case MSG_TYPE::Plain:
				logging::debug(DEBUG_UNFMSG_TAG, fmt::format(DEBUG_UNFMSG_PLAIN,  *(it.plain)));
				break;
			case MSG_TYPE::Image:
				logging::debug(DEBUG_UNFMSG_TAG, fmt::format(DEBUG_UNFMSG_IMAGE,  it.image->get_md5(), it.image->get_id()));
				break;
			default:
				break;
			}
		}
	}
}