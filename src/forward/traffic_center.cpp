
#include "app.h"
#include "structs.h"
#include "traffic_center.h"
#include "unified_message.h"

using namespace std;
using namespace TgBot;

class ForwardItem {
public:
	/* 1 for QQ, 2 for Telegram */
	int32_t type = 1;
	int64_t group_id = 0;

	std::string to_string() {
		return std::to_string(type) + "," + std::to_string(group_id);
	}
};

class ForwardTuple {
public:
	ForwardItem from_group;
	ForwardItem to_group;

	std::string to_string() {
		return from_group.to_string() + " -> " + to_group.to_string();
	}
};

vector<ForwardTuple> getForwardList() {
	vector<ForwardTuple> fwdtpl;
	try {
		vector<string> vecSegTag;
		boost::split(vecSegTag, config.forward_list, boost::is_any_of("|"));
		for (string item : vecSegTag) {
			int direction = 2;
			vector<string> parts;
			if (item.find("<>") != std::string::npos) {
				direction = 2;
				boost::split(parts, item, boost::is_any_of("<>"));
				parts.erase(parts.begin() + 1);
			}
			else if (item.find(">") != std::string::npos) {
				direction = 0;
				boost::split(parts, item, boost::is_any_of(">"));
			}
			else if (item.find("<") != std::string::npos) {
				direction = 1;
				boost::split(parts, item, boost::is_any_of("<"));
			}
			ForwardItem from, to;
			if (parts.size() >= 1) {
				string p = parts.at(0);
				vector<string> forwardItemParts;
				boost::split(forwardItemParts, p, boost::is_any_of(":"));
				if (forwardItemParts.size() == 2) {
					if (forwardItemParts.at(0) == "qq")
						from.type = 1;
					else if (forwardItemParts.at(0) == "tg")
						from.type = 2;
					stringstream ss;
					ss << forwardItemParts.at(1);
					ss >> from.group_id;
				}
			}
			if (parts.size() >= 2) {
				string p = parts.at(1);
				vector<string> forwardItemParts;
				boost::split(forwardItemParts, p, boost::is_any_of(":"));
				if (forwardItemParts.size() == 2) {
					if (forwardItemParts.at(0) == "qq")
						to.type = 1;
					else if (forwardItemParts.at(0) == "tg")
						to.type = 2;
					stringstream ss;
					ss << forwardItemParts.at(1);
					ss >> to.group_id;
				}
			}
			
			if (direction == 0) {
				ForwardTuple tpl;
				tpl.from_group = from;
				tpl.to_group = to;
				fwdtpl.push_back(tpl);
			}
			else if (direction == 1) {
				ForwardTuple tpl;
				tpl.from_group = to;
				tpl.to_group = from;
				fwdtpl.push_back(tpl);
			}
			else if (direction == 2) {
				ForwardTuple tpl;
				tpl.from_group = from;
				tpl.to_group = to;
				fwdtpl.push_back(tpl);
				ForwardTuple tpl2;
				tpl2.from_group = to;
				tpl2.to_group = from;
				fwdtpl.push_back(tpl2);
			}
		}
	}
	catch (exception& e) {
		stringstream ss;
		string error_str;
		ss << u8"解析转发列表出现错误" << e.what();
		ss >> error_str;
		Log::w(u8"API", error_str);
	}
	return fwdtpl;
}

void tg_any_message(Message::Ptr message) {
	try{
		if (forward_enable == 0)
			return;
		UnifiedMessage unimsg = UnifiedMessage();
		unimsg.fillTelegramMessage(message);
		
		if (tg_pre_handler(unimsg) == HANDLER_TERMINATE)
			return;

		auto forward_list = getForwardList();
		for (auto item : forward_list)
			if (item.from_group.type == 2 && item.from_group.group_id == unimsg.from_group_id)
				unimsg.sendToGroup(item.to_group.type, item.to_group.group_id);
	}
	catch (exception& e) {
		Log::e(u8"错误", e.what());
	}
}

void qq_any_message(const json& payload) {
	try {
		if (forward_enable == 0)
			return;
		UnifiedMessage unimsg = UnifiedMessage();
		unimsg.fillQQGroupMessage(payload);

		if (qq_pre_handler(unimsg) == HANDLER_TERMINATE)
			return;

		auto forward_list = getForwardList();
		for (auto item : forward_list)
			if (item.from_group.type == 1 && item.from_group.group_id == unimsg.from_group_id)
				unimsg.sendToGroup(item.to_group.type, item.to_group.group_id);
	}
	catch (exception& e) {
		Log::e(u8"错误", e.what());
	}
}

void parse_qq_private_msg(int32_t sub_type, int32_t msg_id, int64_t from_qq, const string &msg, int32_t font) {
	const auto sub_type_str = [&]() {
		switch (sub_type) {
		case 11:
			return "friend";
		case 1:
			return "other";
		case 2:
			return "group";
		case 3:
			return "discuss";
		default:
			return "unknown";
		}
	}();

	const json payload = {
		{ "post_type", "message" },
		{ "message_type", "private" },
		{ "sub_type", sub_type_str },
		{ "message_id", msg_id },
		{ "user_id", from_qq },
		{ "message", msg },
		{ "font", font }
	};

	qq_any_message(payload);
}

void parse_qq_group_msg(int32_t sub_type, int32_t msg_id, int64_t from_group, int64_t from_qq,
	const string &from_anonymous, const string &msg, int32_t font) {
	string anonymous;
	if (from_qq == 80000000 && !from_anonymous.empty()) {
		const auto anonymous_bin = base64_decode(from_anonymous);
		anonymous = Anonymous::from_bytes(anonymous_bin).name;
	}
	auto is_anonymous = !anonymous.empty();

	const auto sub_type_str = [&]() {
		if (from_qq == 80000000) {
			return "anonymous";
		}
		if (from_qq == 1000000) {
			return "notice";
		}
		if (sub_type == 1) {
			return "normal";
		}
		return "unknown";
	}();

	string final_msg;
	{
		auto prefix = "&#91;" + anonymous + "&#93;:";
		if (!anonymous.empty() && boost::starts_with(msg, prefix)) {
			final_msg = msg.substr(prefix.length());
		}
		else {
			final_msg = move(msg);
		}
	}

	const json payload = {
		{ "post_type", "message" },
		{ "message_type", "group" },
		{ "sub_type", sub_type_str },
		{ "message_id", msg_id },
		{ "group_id", from_group },
		{ "user_id", from_qq },
		{ "anonymous", anonymous },
		{ "anonymous_flag", from_anonymous },
		{ "message", final_msg },
		{ "font", font }
	};

	qq_any_message(payload);
}

void parse_qq_discuss_msg(int32_t sub_type, int32_t msg_id, int64_t from_discuss, int64_t from_qq, const string &msg,
	int32_t font) {
	const json payload = {
		{ "post_type", "message" },
		{ "message_type", "discuss" },
		{ "message_id", msg_id },
		{ "discuss_id", from_discuss },
		{ "user_id", from_qq },
		{ "message", msg },
		{ "font", font }
	};

	qq_any_message(payload);
}