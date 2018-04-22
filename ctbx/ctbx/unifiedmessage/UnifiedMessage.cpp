#include "./UnifiedMessage.h"

namespace types = ctbx::types;
namespace logging = ctbx::logging;
namespace type = ctbx::types;
namespace image = ctbx::image;

using namespace ctbx::cards;

namespace ctbx::message {
	UnifiedMessage::UnifiedMessage(const cq::GroupMessageEvent& cqgmsg) 
		: _unified_card(""), _image_count(0), _from({ types::GROUP_TYPE::QQ, cqgmsg.group_id, Cards::get_card(cqgmsg.group_id, cqgmsg.user_id) }) {
		int64_t from_group_id = cqgmsg.group_id;
		int64_t from_user_id = cqgmsg.user_id;
		cq::Message msg_list(cqgmsg.message);
		for (auto it = msg_list.begin(); it != msg_list.end(); it++) {
			if (it->type == "at") {
					int64_t reply_qq = std::stoll(it->data.at("qq"));
					_segs.emplace_back(
					new UReply({
							types::GROUP_TYPE::QQ,
							reply_qq,
							Cards::get_card(from_group_id, reply_qq)
							}));
					msg_list.erase(it);
					break;
			}
		}
		bool img_only = true;
		for(auto&it : msg_list)
			if (it.type != "image") {
				img_only = false;
				break;
			}
		for (auto it = msg_list.begin(); it != msg_list.end();) {
			if (it->type == "image") {
				_segs.emplace_back(new UImage(it->data.at("file")));
				if (!img_only) {
					it->type = "text";
					_image_count++;
					it->data["text"] = "[图片" + std::to_string(_image_count) + "]";
				}
				else
					it = msg_list.erase(it);
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
	UnifiedMessage::UnifiedMessage(const TgBot::Message::Ptr& tgmsg, const TgBot::Bot& tgbot) 
		: _unified_card(""), _image_count(0), _from({ types::GROUP_TYPE::TG,tgmsg->from->id,tgmsg->from->firstName}) {
		if (tgmsg->forwardFrom.use_count()) {
			_segs.emplace_back(new UForward({ types::GROUP_TYPE::TG,tgmsg->forwardFrom->id,tgmsg->forwardFrom->firstName }));
		}
		if (tgmsg->replyToMessage.use_count()) {
			_segs.emplace_back(new UReply({ types::GROUP_TYPE::TG, tgmsg->replyToMessage->from->id, tgmsg->replyToMessage->from->firstName }));
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
			_segs.emplace_back(new UPlain(tgmsg->text));
		}
	}
	void UnifiedMessage::send(const ctbx::types::Group& group, const TgBot::Bot& bot){
		if (group.type == types::GROUP_TYPE::QQ)
			send_to_qq(group.group_id, bot);
		else
			send_to_tg(group.group_id, bot);
	}
	void UnifiedMessage::send_to_qq(const int64_t group_id, const TgBot::Bot& bot){
		if (_unified_card.empty())
			_unified_card = _parse_card();
		short image_index = 1;
		for (auto& it : _segs) {
			if (it.type == MSG_TYPE::Image) {
				it.image->send_to_qq(group_id, bot, _unified_card + "(" + std::to_string(image_index) + "/" + std::to_string(_image_count) + ")");
				image_index++;
			}
		}
		for (auto& it : _segs) {
			if (it.type == MSG_TYPE::Plain) {
				std::string text = _unified_card + *(it.plain);
				cq::Message msg = text;
				try {
					msg.send(cq::Target(group_id, cq::Target::Type::GROUP));
				}
				catch (const cq::exception::ApiError& e) {
					logging::error(u8"UnifiedMessage", u8"消息:\"" + text + u8"\"发送失败，错误原因:" + std::string(e.what()));
				}
			}
		}
	}
	void UnifiedMessage::send_to_tg(const int64_t group_id, const TgBot::Bot & bot){
		if (_unified_card.empty())
			_unified_card = _parse_card();
		short image_index = 1;
		for (auto& it : _segs) {
			if (it.type == MSG_TYPE::Image) {
				std::string cap = _unified_card + "(" + std::to_string(image_index) + "/" + std::to_string(_image_count) + ")";
				it.image->send_to_tg(group_id, bot, cap);
				image_index++;
			}
		}
		for (auto& it : _segs) {
			if (it.type == MSG_TYPE::Plain) {
				std::string text = _unified_card + *(it.plain);
				try {
					bot.getApi().sendMessage(group_id, text);
				}
				catch (const TgBot::TgException::exception& e) {
					logging::error(u8"UnifiedMessage", u8"消息:\"" + text + u8"\"发送失败，错误原因:" + std::string(e.what()));
				}
			}
		}
	}
	std::string UnifiedMessage::_parse_card(){
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
	void UnifiedMessage::_debug_remaining_msg(const cq::Message& msg) {
		logging::debug(u8"UnifiedMessage", "检查CQ剩余消息");
		if (msg.empty())
			logging::debug(u8"UnifiedMessage", "全部处理完成");
		else
			logging::debug(u8"UnifiedMessage", "仍有消息未处理");
		for (auto& it : msg) {
			logging::debug(u8"UnifiedMessage", "type:" + it.type + " data:" + it.data.begin()->second);
		}
	}
	void UnifiedMessage::_debug_all_segs() {
		logging::debug(u8"UnifiedMessage", "打印信息片段");
		for (auto& it : _segs) {
			switch (it.type){
			case MSG_TYPE::Forward:
				logging::debug(u8"UnifiedMessage", u8"Forward : " + it.forward->card);
				break;
			case MSG_TYPE::Reply:
				logging::debug(u8"UnifiedMessage", u8"Reply : " + it.reply->card);
				break;
			case MSG_TYPE::Plain:
				logging::debug(u8"UnifiedMessage", u8"Plain : " + *(it.plain));
				break;
			case MSG_TYPE::Image:
				logging::debug(u8"UnifiedMessage", u8"Image : md5="  + it.image->get_md5() + " id=" + it.image->get_id());
				break;
			default:
				break;
			}
		}
	}
}