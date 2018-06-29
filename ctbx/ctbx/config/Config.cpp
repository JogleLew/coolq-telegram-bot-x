#include "./Config.h"

namespace logging = ctbx::logging;

namespace ctbx::config {
	const std::string config_relative_path = "config\\config.json";
	
	Config& Config::get_config() {
		static Config singleton_config;
		return singleton_config;
	}

	Config::~Config() { logging::logger_drop(); }

	bool Config::is_valid()const { return _is_valid; }

	int64_t Config::get_bot_qq()const { return _bot.QQ; };

	const std::string Config::get_bot_token()const { return _bot.token; }

	const std::string Config::get_debug_level()const { return _debug_level; }

	const std::vector<Group> Config::get_forward_groups(const Group& group){ 
		auto v = _group_to_group[group];
		return v;
	}

	const std::vector<Group> Config::get_all_from_groups(){
		std::vector<Group> ans;
		for (auto &it : _group_to_group)
			ans.push_back(it.first);
		return ans;
	}

	void Config::reload() {
		_initialize();
		std::fstream in(_config_path, std::ios::in);
		if (in.is_open()) {
			try {
				in >> _config;
				_read();
				_is_valid = true;
			}
			catch (const Json::Exception& e) {
				logging::debug(u8"Jsoncpp", u8"读取错误：" + std::string(e.what()));
				_is_valid = false;
				return;
			}
			catch (const std::exception& e) {
				logging::debug(u8"Config", u8"读取错误：" + std::string(e.what()));
				_is_valid = false;
				return;
			}
		}
		else
			_is_valid = false;
	}

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

	Config::Config() : _config_path(cq::utils::ansi(cq::dir::app()) + config_relative_path) {
		_initialize();
	}

	void Config::_add_forward(const Group& from_group, const Group& to_group) {
		_group_to_group[from_group].push_back(to_group);
	}

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
			if (type == "TG<->QG") {
				_add_forward(Group({ SOFTWARE_TYPE::TG, from_id }), Group({ SOFTWARE_TYPE::QQ, to_id }));
				_add_forward(Group({ SOFTWARE_TYPE::QQ, to_id }), Group({ SOFTWARE_TYPE::TG, from_id }));
			}
			else if (type == "QG<->TG") {
				_add_forward(Group({ SOFTWARE_TYPE::QQ, from_id }), Group({ SOFTWARE_TYPE::TG, to_id }));
				_add_forward(Group({ SOFTWARE_TYPE::TG, to_id }), Group({ SOFTWARE_TYPE::QQ, from_id }));
			}
			else if(type == "TG->QG")
				_add_forward(Group({ SOFTWARE_TYPE::TG, from_id }), Group({ SOFTWARE_TYPE::QQ, to_id }));
			else if(type == "QG->TG")
				_add_forward(Group({ SOFTWARE_TYPE::QQ, from_id }), Group({ SOFTWARE_TYPE::TG, to_id }));
			else if(type == "TG->TG")
				_add_forward(Group({ SOFTWARE_TYPE::TG, from_id }), Group({ SOFTWARE_TYPE::TG, to_id }));
			else if(type == "QG->QG")
				_add_forward(Group({ SOFTWARE_TYPE::QQ, from_id }), Group({ SOFTWARE_TYPE::QQ, to_id }));
			// and more forwarding will be supported in near(maybe?) future.
		}
	}
}