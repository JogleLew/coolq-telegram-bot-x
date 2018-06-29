#pragma once

#include "ctbx/stdafx.h"
#include "ctbx/logging/Logging.h"
#include "ctbx/types/Types.h"

namespace ctbx::config {
	using ctbx::types::SOFTWARE_TYPE;
	using ctbx::types::Group;
	using ctbx::types::User;

	typedef struct _BOT {
		int64_t QQ = 0;
		std::string token = "INVALID_TOKEN";
	} Bot, *PBot;

	class Config {

	private:
		const std::string _config_path;

		std::string _debug_level;
		Json::Value _config;
		bool _is_valid;
		Bot _bot;

		std::map < Group, std::vector<Group>> _group_to_group;

		// unimplemented.
		std::map < User, std::vector<User> > _user_to_user;
		std::map < Group, std::vector<User> > _group_to_user;
		std::map< User, std::vector<Group> > _user_to_group;

	public:
		static Config& get_config();
		bool is_valid()const;
		int64_t get_bot_qq() const;
		const std::string get_bot_token()const;
		const std::string get_debug_level()const;
		const std::vector<Group> get_forward_groups(const Group&);
		const std::vector<Group> get_all_from_groups();
		void reload();
	private:
		explicit Config();
		~Config();
		void _add_forward(const Group&, const Group&);
		void _read();
		void _initialize();
	};
}