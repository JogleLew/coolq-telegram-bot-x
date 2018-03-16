#include "./utils.h"

#include<memory>
#include<string>

namespace ctbx::utils::logging {
	static std::shared_ptr<spdlog::logger> logger(static_cast<spdlog::logger*>(0));
	void logger_initialize(const std::string& app_dir, int hour, int minute) {
		logger = spdlog::daily_logger_mt(logger_name, app_dir, hour, minute);
		logger->set_pattern("[%Y-%m-%d %H:%M:%S] [%l] %v");
		logger->flush_on(spdlog::level::level_enum::trace);
		logger->set_level(spdlog::level::level_enum::debug);
	}
	void log(const spdlog::level::level_enum level, const std::string& tag, const std::string& msg) {logger->log(level, "[{}] [{}]", tag, msg);}
	void debug(const std::string& tag, const std::string& msg) { log(spdlog::level::debug, tag, msg); }
	void info(const std::string& tag, const std::string& msg) { log(spdlog::level::info, tag, msg); }
	void error(const std::string& tag, const std::string& msg) { log(spdlog::level::err, tag, msg); }
	void warning(const std::string& tag, const std::string& msg) { log(spdlog::level::warn, tag, msg); }
	void fatal(const std::string& tag, const std::string& msg) { log(spdlog::level::critical, tag, msg); }
	void logger_drop() { spdlog::drop(logger_name); }
}