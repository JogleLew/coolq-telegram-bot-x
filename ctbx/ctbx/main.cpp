#include "./Bot.h"

namespace app = cq::app; 
namespace event = cq::event;
namespace logging = cq::logging;

CQ_INITIALIZE("top.jogle.ctbx");

static std::shared_ptr<ctbx::Bot> pbot(static_cast<ctbx::Bot*>(0));

CQ_MAIN{
cq::config.convert_unicode_emoji = true; 

app::on_coolq_start = []() {
	ctbx::logging::logger_initialize(cq::dir::app() + "ctbxlog.txt");
};

app::on_enable = []() {
	std::string app_dir = cq::dir::app();
	pbot = std::shared_ptr<ctbx::Bot>(new ctbx::Bot(app_dir));
	if (pbot->config_valid()) {
		pbot->bot_on_enable();
		logging::info(u8"CTBX", u8"Bot已启动，log文件请到APP文件夹查看");
	}
	else {
		logging::error(u8"CTBX", u8"Bot配置文件无效，请修改配置文件后重启Bot");
		pbot.reset();
	}
};

event::on_group_msg = [](const auto& e) {
	if(pbot.use_count())
		pbot->qq_receive_groupmessage(e); 
};

app::on_coolq_exit = []() {
	if (pbot.use_count()) {
		pbot.reset();
		logging::info(u8"CTBX", u8"Bot已停止");
	}
};

app::on_disable = []() {
	app::on_coolq_exit();
	ctbx::logging::logger_drop();
};

}