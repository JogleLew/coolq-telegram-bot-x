
#include "app.h"
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

void qq_private_message(int32_t sub_type, int32_t msg_id, int64_t from_qq, string msg, int32_t font) {
	
}

void qq_group_message(int32_t sub_type, int32_t msg_id, int64_t from_group, int64_t from_qq, string from_anonymous, string msg, int32_t font) {
	try {
		if (forward_enable == 0)
			return;
		UnifiedMessage unimsg = UnifiedMessage();
		unimsg.fillQQGroupMessage(sub_type, msg_id, from_group, from_qq, from_anonymous, msg, font);

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

void qq_discuss_message(int32_t msg_id, int64_t from_discuss, int64_t from_qq, string msg, int32_t font) {

}