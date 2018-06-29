#include "./Bot.h"

namespace app = cq::app; 
namespace event = cq::event;
namespace config = ctbx::config;

using config::Config;

CQ_INITIALIZE("top.jogle.ctbx");

static std::shared_ptr<ctbx::Bot> pbot(static_cast<ctbx::Bot*>(0));

bool logger_existed = false;

CQ_MAIN{
cq::config.convert_unicode_emoji = true; 

app::on_coolq_start = []() {

};

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

event::on_group_msg = [](const auto& e) {
	if(pbot.use_count())
		pbot->qq_receive_groupmessage(e); 
};

app::on_disable = []() {
	if (pbot.use_count()) {
		pbot.reset();
		cq::logging::info(u8"CTBX", u8"Bot已停止");
	}
};


app::on_coolq_exit = []() {
	app::on_disable();
	if (logger_existed) {
		ctbx::logging::logger_drop();
	}
};

}