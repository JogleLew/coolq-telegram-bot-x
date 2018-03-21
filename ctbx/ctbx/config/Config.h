#pragma once

#include "ctbx/stdafx.h"
#include "ctbx/logging/Logging.h"
#include "ctbx/types/Types.h"

namespace ctbx::config {
	using ctbx::types::GROUP_TYPE;
	using ctbx::types::Group;
	typedef struct _BOT {
		int64_t QQ = 0;
		std::string token = "INVALID_TOKEN";
	} Bot, *PBot;
	class Config {
	public:
		static const std::string file_path;
	private:
		Json::Value _config;
		bool _is_valid;
		Bot _bot;
		std::map < Group, std::vector<Group>> _forward_list;
		std::mutex _forward_list_mtx;
	public:
		explicit Config(const std::string&);
		~Config();
		bool is_valid()const;
		int64_t get_bot_qq() const;
		const std::string get_bot_token()const;
		void add_forward(const Group&, const Group&);
		const std::vector<Group> get_forward_groups(const Group&);
		const std::vector<Group> get_all_from_groups();
	private:
		void _read_and_check();
	};
}