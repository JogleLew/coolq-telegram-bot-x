/// @file    Localization.cpp
/// @brief   Localization config and localization string
/// @author  JogleLew
/// @date    2018-08-07
/// @note    localization settings: change language, and get localization text.
///

#include "./Localization.h"

namespace logging = cq::logging;

namespace ctbx::localization {
	std::string language;
	Json::Value localized_str;

	/// _zh_cn
	/// @note   initialize zh_CN string
	/// @return void
	/// 
	void _zh_cn() {
		// This method should be replace by reading localization file in the future. 
		localized_str.clear();
		localized_str["LOG_INIT_FAILED_REASON"] = u8"spdlog初始化失败，原因：";
		localized_str["LOG_INIT_FAILED"] = u8"日志初始化失败，是不是没有给写入权限？";
		localized_str["BOT_STARTED"] = u8"Bot已启动，log文件请到APP文件夹查看";
		localized_str["BOT_CONFIG_INVALID"] = u8"Bot配置文件无效，请修改配置文件后重启Bot";
		localized_str["BOT_STOPED"] = u8"Bot已停止";
	}

	/// _en_us
	/// @note   initialize en_US string
	/// @return void
	/// 
	void _en_us() {
		// This method should be replace by reading localization file in the future. 
		localized_str.clear();
		localized_str["LOG_INIT_FAILED_REASON"] = u8"Failed to initialize spdlog, reason: ";
		localized_str["LOG_INIT_FAILED"] = u8"Failed to initialize logger, please check file or directory permission.";
		localized_str["BOT_STARTED"] = u8"Bot is started, log file is in app folder";
		localized_str["BOT_CONFIG_INVALID"] = u8"Config file is invalid, please correct it and restart the bot";
		localized_str["BOT_STOPED"] = u8"Bot is stopped";
	}

	/// locale_initialize
	/// @note   initialize language
	/// @return void
	/// 
	void locale_initialize() {
		set_language("zh_CN");
	}

	/// set_language
	/// @note   set up certain language
	/// @param  lang Language 
	/// @return void
	/// 
	void set_language(const std::string& lang) {
		language = lang;
		if (lang == "zh-CN")
			_zh_cn();
		else if (lang == "en-US") 
			_en_us();
		else {
			logging::warning(u8"Localization", "Unsupported language, fallback to en_US.");
			_en_us();
		}
	}

	/// str
	/// @note   get localized string by key
	/// @param  key Key name 
	/// @return Localized string
	/// 
	std::string str(const std::string& key) {
		return localized_str.get(key, "").asString();
	}
}