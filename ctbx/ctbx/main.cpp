/// @file    main.cpp
/// @brief   CTBX Main Entry
/// @author  wtdcode
/// @date    2018-03-21
/// @note    The main entry of CoolQ Telegram Bot X.
///

#include "./Bot.h"

namespace app    = cq::app; 
namespace event  = cq::event;
namespace config = ctbx::config;
namespace locale = ctbx::localization;

using config::Config;

CQ_INITIALIZE("top.jogle.ctbx"); ///< initialize CQSDK

static std::shared_ptr<ctbx::Bot> pbot(static_cast<ctbx::Bot*>(0)); ///< initialize forward control 

bool logger_existed = false; ///< status of logger existence

CQ_MAIN{
cq::config.convert_unicode_emoji = true; 

/// on_coolq_start
/// @note This function will execute when CoolQ starts.
///
app::on_coolq_start = []() {
	locale::locale_initialize(); ///< initialize localization string
};

/// on_enable
/// @note This function will execute when CoolQ SDK is enabled.
///
app::on_enable = []() {
	std::string app_dir = cq::utils::ansi(cq::dir::app());
	if (!logger_existed) {
		try {
			ctbx::logging::logger_initialize(app_dir + "ctbx.log");
			logger_existed = true;
		}
		catch (spdlog::spdlog_ex& e) {
			cq::logging::debug(u8"CTBX", u8"spdlog初始化失败，原因：" + std::string(e.what()));
			cq::logging::warning(u8"CTBX", u8"日志初始化失败，是不是没有给写入权限？");
			pbot.reset();
			logger_existed = false;
			return;
		}
	}
	Config& config = Config::get_config();
	config.reload();
	if(config.is_valid()){
		pbot = std::shared_ptr<ctbx::Bot>(new ctbx::Bot());
		pbot->bot_on_enable();
		cq::logging::info(u8"CTBX", u8"Bot已启动，log文件请到APP文件夹查看");
	}
	else {
		cq::logging::error(u8"CTBX", u8"Bot配置文件无效，请修改配置文件后重启Bot");
		pbot.reset();
	}
};

/// on_group_msg
/// @note This function will execute when CoolQ SDK receives group message.
///
event::on_group_msg = [](const auto& e) {
	if(pbot.use_count())
		pbot->qq_receive_groupmessage(e); 
};

/// on_disable
/// @note This function will execute when CoolQ SDK is disabled.
///
app::on_disable = []() {
	if (pbot.use_count()) {
		pbot.reset();
		cq::logging::info(u8"CTBX", u8"Bot已停止");
	}
};

/// on_coolq_exit
/// @note This function will execute when CoolQ exits.
/// 
app::on_coolq_exit = []() {
	app::on_disable();
	if (logger_existed) {
		ctbx::logging::logger_drop();
	}
};

}