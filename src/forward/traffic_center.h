#pragma once

#include "tgbot\tgbot.h"
#include "pre_handler.h"

using namespace std;

void tg_any_message(TgBot::Message::Ptr message);

void qq_private_message(int32_t sub_type, int32_t msg_id, int64_t from_qq, string msg, int32_t font);
void qq_group_message(int32_t sub_type, int32_t msg_id, int64_t from_group, int64_t from_qq, string from_anonymous, string final_msg, int32_t font);
void qq_discuss_message(int32_t msg_id, int64_t from_discuss, int64_t from_qq, string msg, int32_t font);