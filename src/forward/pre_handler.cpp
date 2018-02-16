
#include "pre_handler.h"

int HANDLER_PASS = 0;
int HANDLER_TERMINATE = 1;

int tg_pre_handler(UnifiedMessage message) {
	if (showid_handler(message) == HANDLER_TERMINATE)
		return HANDLER_TERMINATE;
	if (ali_handler(message) == HANDLER_TERMINATE)
		return HANDLER_TERMINATE;
	return HANDLER_PASS;
}

int qq_pre_handler(UnifiedMessage message) {
	if (showid_handler(message) == HANDLER_TERMINATE)
		return HANDLER_TERMINATE;
	if (ali_handler(message) == HANDLER_TERMINATE)
		return HANDLER_TERMINATE;
	return HANDLER_PASS;
}

/* Pre-handlers define as below */
int showid_handler(UnifiedMessage message) {
	if (message.message_type == 1 && message.message_content == "!!showid") {
		std::string group_id = std::to_string(message.from_group_id);
		if (message.message_source == 1)
			sdk->send_group_msg(message.from_group_id, group_id);
		else if (message.message_source == 2)
			tgbot->getApi().sendMessage(message.from_group_id, group_id);
		return HANDLER_TERMINATE;
	}
	return HANDLER_PASS;
}

int ali_handler(UnifiedMessage message) {
	if (message.message_type == 1 && message.message_content == "!!ali") {
		std::string red_packet_link = "http://jogle.top/hb";
		if (message.message_source == 1)
			sdk->send_group_msg(message.from_group_id, red_packet_link);
		else if (message.message_source == 2)
			tgbot->getApi().sendMessage(message.from_group_id, red_packet_link);
		return HANDLER_TERMINATE;
	}
	return HANDLER_PASS;
}