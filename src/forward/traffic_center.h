#pragma once

#include "tgbot\tgbot.h"
#include "pre_handler.h"

using namespace std;

void tg_any_message(TgBot::Message::Ptr message);

void qq_any_message(const json& payload);

void parse_qq_private_msg(int32_t sub_type, int32_t msg_id, int64_t from_qq, const string &msg, int32_t font);
void parse_qq_group_msg(int32_t sub_type, int32_t msg_id, int64_t from_group, int64_t from_qq, const string &from_anonymous, const string &msg, int32_t font);
void parse_qq_discuss_msg(int32_t sub_type, int32_t msg_id, int64_t from_discuss, int64_t from_qq, const string &msg, int32_t font);