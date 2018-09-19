/// @file    Config.cpp
/// @brief   CTBX Config Control
/// @author  wtdcode
/// @date    2018-03-21
/// @note    Config management: load and save
///

#include "./Config.h"

namespace logging = ctbx::logging;

using namespace ctbx::logging::config;
using namespace cq::utils;

namespace ctbx::config {
	const std::string config_relative_path = "config\\config.json";
	
	/// get_config
	/// @note   Get static config file
	/// @return Config file
	/// 
	Config& Config::get_config() {
		static Config singleton_config;
		return singleton_config;
	}

	/// Destructor
	/// @note Destructor of Config class
	/// 
	Config::~Config() { logging::logger_drop(); }

	/// is_valid
	/// @note   Getter of _is_valid
	/// @return Boolean value indicates config validation
	/// 
	bool Config::is_valid()const { return _is_valid; }

	/// get_bot_qq
	/// @note   Getter of bot QQ
	/// @return Bot QQ id
	///
	int64_t Config::get_bot_qq()const { return _bot.QQ; };

	/// get_bot_token
	/// @note   Getter of bot token
	/// @return Bot token
	///
	const std::string Config::get_bot_token()const { return _bot.token; }

	/// get_debug_level
	/// @note   Getter of debug level
	/// @return Debug level
	///
	const std::string Config::get_debug_level()const { return _debug_level; }

	/// get_forward_groups
	/// @note   Get forward destinations of one certain group
	/// @param  group Group info
	/// @return Forward destination of groups
	///
	const std::vector<Group> Config::get_forward_groups(const Group& group){ 
		auto v = _group_to_group[group];
		return v;
	}

	/// get_all_from_groups
	/// @note   Get all groups which needs to forward
	/// @return Forward source of groups
	///
	const std::vector<Group> Config::get_all_from_groups(){
		std::vector<Group> ans;
		for (auto &it : _group_to_group)
			ans.push_back(it.first);
		return ans;
	}

	/// reload
	/// @note   Reload config file
	/// @return void
	///
	void Config::reload() {
		_initialize();
		std::fstream in(_config_path, std::ios::in);
		if (in.is_open()) {
			try {
				in >> _config;
				_read();
				_is_valid = true;
			}
			catch (const std::exception& e) {
				logging::debug(DEBUG_CTBX_CONFIG_TAG, fmt::format(DEBUG_CTBX_CONFIG_READ, string_decode(std::string(e.what()), Encoding::ANSI)));
				_is_valid = false;
				return;
			}
		}
		else
			_is_valid = false;
	}

	/// _initialize
	/// @note   Initialize config file
	/// @return void
	///
	void Config::_initialize() {
		_config.clear();
		_is_valid = false;
		_debug_level.clear();
		_bot.QQ = 0;
		_bot.token = "INVALID_TOKEN";
		_group_to_group.clear();
		_user_to_user.clear();
		_group_to_user.clear();
		_user_to_group.clear();
	}

	/// Constructor
	/// @note Constructor of Config class
	/// 
	Config::Config() : _config_path(cq::utils::ansi(cq::dir::app()) + config_relative_path) {
		_initialize();
	}

	/// _add_forward
	/// @note   Add forward item
	/// @param  from_group Source group
	/// @param  to_group   Destination group
	/// @return void
	/// 
	void Config::_add_forward(const Group& from_group, const Group& to_group) {
		_group_to_group[from_group].push_back(to_group);
	}

	/// _read
	/// @note   Read config file
	/// @return void
	///
	void Config::_read() throw(Json::Exception){
		_bot.token = _config["Bot_Token"].asString();
		_bot.QQ = _config["Bot_QQ"].asInt64();
		_debug_level = _config.get("Debug_Level", "debug").asString();
		for (auto &it : _config["Forward_List"]) {
			std::string type = it["Type"].asString();
			std::size_t type_len = type.size();
			int64_t from_id = it["From"].asInt64();
			int64_t to_id = it["To"].asInt64();
			// C++ doesn't support string switch, sad.
			if (type == "T<->Q") {
				_add_forward(Group({ SOFTWARE_TYPE::TG, from_id }), Group({ SOFTWARE_TYPE::QQ, to_id }));
				_add_forward(Group({ SOFTWARE_TYPE::QQ, to_id }), Group({ SOFTWARE_TYPE::TG, from_id }));
			}
			else if (type == "Q<->T") {
				_add_forward(Group({ SOFTWARE_TYPE::QQ, from_id }), Group({ SOFTWARE_TYPE::TG, to_id }));
				_add_forward(Group({ SOFTWARE_TYPE::TG, to_id }), Group({ SOFTWARE_TYPE::QQ, from_id }));
			}
			else if(type == "T->Q")
				_add_forward(Group({ SOFTWARE_TYPE::TG, from_id }), Group({ SOFTWARE_TYPE::QQ, to_id }));
			else if(type == "Q->T")
				_add_forward(Group({ SOFTWARE_TYPE::QQ, from_id }), Group({ SOFTWARE_TYPE::TG, to_id }));
			else if(type == "T->T")
				_add_forward(Group({ SOFTWARE_TYPE::TG, from_id }), Group({ SOFTWARE_TYPE::TG, to_id }));
			else if(type == "Q->Q")
				_add_forward(Group({ SOFTWARE_TYPE::QQ, from_id }), Group({ SOFTWARE_TYPE::QQ, to_id }));
			// and more forwarding will be supported in near(maybe?) future.
		}
	}
}