#include "./UnifiedMessage.h"

namespace types = ctbx::types;
namespace logging = ctbx::logging;
namespace type = ctbx::types;
namespace image = ctbx::image;

using namespace ctbx::cards;

namespace ctbx::message {
	UnifiedMessage::UnifiedMessage(const cq::GroupMessageEvent& cqgmsg){
		int64_t from_group_id = cqgmsg.group_id;
		int64_t from_user_id = cqgmsg.user_id;
		_from = std::shared_ptr<types::User>(new types::User({ types::GROUP_TYPE::QQ,
															   from_user_id ,
															   Cards::get_card(from_group_id, from_user_id)}));
		for (auto &it : cqgmsg.message) {
			if (it.type == "at") {
				int64_t atqq = std::stoll(it.data.at("qq"));
				_reply_to = std::shared_ptr<types::User>(new types::User({types::GROUP_TYPE::QQ,
																		  atqq,
																		  Cards::get_card(atqq, from_group_id)}));
			}
			if (it.type == "text") {
				_plain_text = it.data.at("text");
			}
			if (it.type == "image") {
				_images.push_back(image::Image::get_image(it.data.at("file")));
			}
		}
	}
	UnifiedMessage::UnifiedMessage(const TgBot::Message::Ptr& tgmsg){
		_from = std::shared_ptr<types::User>(new types::User({types::GROUP_TYPE::TG,
															  tgmsg->from->id,
															  tgmsg->from->username }));
		if (tgmsg->text != "")
			_plain_text = tgmsg->text;
		if (tgmsg->forwardFrom.use_count())
			_forward_from = std::shared_ptr<types::User>(new types::User({types::GROUP_TYPE::TG,
																	      tgmsg->forwardFrom->id,
																	      tgmsg->forwardFrom->username }));
		if (tgmsg->replyToMessage.use_count()) {
			_replied_message = std::shared_ptr<UnifiedMessage>(new UnifiedMessage(tgmsg->replyToMessage));
			_reply_to = std::shared_ptr<types::User>(new types::User({ types::GROUP_TYPE::TG,
																	  tgmsg->replyToMessage->from->id,
																	  tgmsg->replyToMessage->from->username }));
		}
	}
	void UnifiedMessage::send(const ctbx::types::Group& group, const TgBot::Bot& bot){
		if (group.type == types::GROUP_TYPE::QQ)
			send_to_qq(group.group_id);
		else
			send_to_tg(group.group_id, bot);
	}
	void UnifiedMessage::send_to_qq(const int64_t group_id){
		// TODO : add a mutex?
		std::string text = _parse_text();
		cq::Message msg = text;
		try {
			msg.send(cq::Target(group_id, cq::Target::Type::GROUP));
		}
		catch (const cq::exception::ApiError& e) {
			logging::error(u8"UnifiedMessage", u8"消息:\"" + text + u8"\"发送失败，错误原因:" + std::string(e.what()));
		}
	}
	void UnifiedMessage::send_to_tg(const int64_t group_id, const TgBot::Bot & bot){
		if (!_images.empty()) {
			for (std::size_t i = 1; i <= _images.size();i++)
				_images[i-1].send_to_tg(group_id, bot,  std::to_string(i) + "/" + std::to_string(_images.size()));
		}
		std::string text = _parse_text();
		try {
			bot.getApi().sendMessage(group_id, text);
		}
		catch (const TgBot::TgException::exception& e) {
			logging::error(u8"UnifiedMessage", u8"消息:\"" + text + u8"\"发送失败，错误原因:" + std::string(e.what()));
		}
	}
	std::string UnifiedMessage::_parse_text(){
		std::string text = "";
		text += _from->card;
		if (_forward_from.use_count())
			text += ("(Forward from " + _forward_from->card + ")");
		if (_reply_to.use_count()) {
			text += ("(=>" + _reply_to->card);
			if (_replied_message.use_count())
				text += (":" + _replied_message->_plain_text);
			text += ")";
		}
		text += (":" + _plain_text);
		return text;
	}
}