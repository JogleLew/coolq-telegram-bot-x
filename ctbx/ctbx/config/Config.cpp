#include "./Config.h"

namespace logging = ctbx::logging;

namespace ctbx::config {
	const std::string Config::file_path = "config\\config.json";
	Config::Config(const std::string& app_dir) {
		std::string config_path = app_dir  + file_path;
		std::fstream in(config_path, std::ios::in);
		if (in.is_open()) {
			try {
				in >> _config;
				_read_and_check();
			}
			catch (const Json::Exception& e) {
				logging::debug(u8"Jsoncpp", u8"读取错误：" + std::string(e.what()));
				_is_valid = false;
				return;
			}
		}
		else 
			_is_valid = false;
	}
	Config::~Config() { logging::logger_drop(); }
	bool Config::is_valid()const { return _is_valid; }
	int64_t Config::get_bot_qq()const { return _bot.QQ; };
	const std::string Config::get_bot_token()const { return _bot.token; }
	const std::vector<Group> Config::get_forward_groups(const Group& group){ 
		_forward_list_mtx.lock();
		auto v = _forward_list[group];
		_forward_list_mtx.unlock();
		return v;
	}
	const std::vector<Group> Config::get_all_from_groups(){
		_forward_list_mtx.lock();
		std::vector<Group> ans;
		for (auto &it : _forward_list)
			ans.push_back(it.first);
		_forward_list_mtx.unlock();
		return ans;
	}
	void Config::add_forward(const Group& from_group, const Group& to_group) { 
		_forward_list_mtx.lock();
		_forward_list[from_group].push_back(to_group); 
		_forward_list_mtx.unlock();
	}
	void Config::_read_and_check() throw(Json::Exception){
		_bot.token = _config["Bot_Token"].asString();
		_bot.QQ = _config["Bot_QQ"].asInt64();
		for (auto &it : _config["Forward_List"]) {
			std::string type = it["Type"].asString();
			std::size_t type_len = type.size();
			int64_t from_id = it["From_Group"].asInt64();
			int64_t to_id = it["To_Group"].asInt64();
			// C++ doesn't support string switch, sad.
			if (type == "T<->Q") {
				_forward_list[Group({ GROUP_TYPE::TG, from_id })].push_back(Group({ GROUP_TYPE::QQ, to_id }));
				_forward_list[Group({ GROUP_TYPE::QQ, to_id })].push_back(Group({ GROUP_TYPE::TG, from_id }));
			}
			else if (type == "Q<->T") {
				_forward_list[Group({ GROUP_TYPE::QQ, from_id })].push_back(Group({ GROUP_TYPE::TG, to_id }));
				_forward_list[Group({ GROUP_TYPE::TG, to_id })].push_back(Group({ GROUP_TYPE::QQ, from_id }));
			}
			else if(type == "T->Q")
				_forward_list[Group({ GROUP_TYPE::TG, from_id })].push_back(Group({ GROUP_TYPE::QQ, to_id }));
			else if(type == "Q->T")
				_forward_list[Group({ GROUP_TYPE::QQ, from_id })].push_back(Group({ GROUP_TYPE::TG, to_id }));
			else if(type == "T->T")
				_forward_list[Group({ GROUP_TYPE::TG, from_id })].push_back(Group({ GROUP_TYPE::TG, to_id }));
			else if(type == "Q->Q")
				_forward_list[Group({ GROUP_TYPE::QQ, from_id })].push_back(Group({ GROUP_TYPE::QQ, to_id }));
		}
		if (_forward_list.empty())
			_is_valid = false;
		else
			_is_valid = true;
		return;
	}
}