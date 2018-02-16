/*
 * Copyright (c) 2015 Oleg Morozenkov
 *
 */

#ifndef TGBOT_CPP_CHAT_H
#define TGBOT_CPP_CHAT_H

#include <string>
#include <memory>

namespace TgBot {

/**
 * This object represents a Telegram Chat
 * @ingroup types
 */
class Chat {

public:
	typedef std::shared_ptr<Chat> Ptr;

	/**
	 * Enum of possible types of a chat.
	 */
	enum class Type {
		Private, Group, Supergroup, Channel
	};

	/**
	 * Unique identifier for this chat, not exceeding 1e13 by absolute value
	 */
	int64_t id;

	/**
	 * Type of chat: can be either "private", "group", "supergroup, or "channel".
	 */
	Type type;

	/**
	 * Optional. Title for channels and group chat
	 */
	std::string title;

	/**
	 * Optional. Username for
	 * private chats and channels
	 */
	std::string username;

	/**
	 * Optional. First name of the
	 * other party in private chat
	 */
	std::string firstName;

	/**
	 * Optional. Last name of the
	 * other party in private chat
	 */
	std::string lastName;

	/**
	 * Optional. True if a group 
	 * has �All Members Are Admins� enabled.
	 */
	bool allMembersAreAdministrators;
};

}

#endif //TGBOT_CPP_CHAT_H
