#pragma once

#include "ctbx/stdafx.h"
#include "Locale.h"

namespace ctbx::logging {
	static const std::string logger_name = "logger";
	static const std::map<std::string, spdlog::level::level_enum> levels = {
		{"debug", spdlog::level::level_enum::debug}, 
		{"info", spdlog::level::level_enum::info},
		{"warning",  spdlog::level::level_enum::warn}, 
		{"error",  spdlog::level::level_enum::err },
		{"critical", spdlog::level::level_enum::critical},
		{"off", spdlog::level::level_enum::off},
		{"trace", spdlog::level::level_enum::trace}
	};
	static std::shared_ptr<spdlog::logger> logger(static_cast<spdlog::logger*>(0));
	inline void logger_initialize(const std::string& app_dir, int hour = 0, int minute = 0) {
		logger = spdlog::daily_logger_mt(logger_name, app_dir, hour, minute);
		logger->set_pattern("[%Y-%m-%d %H:%M:%S] [%l] %v");
		logger->flush_on(spdlog::level::level_enum::trace);
		logger->set_level(spdlog::level::level_enum::debug);
	}
	inline void log(const spdlog::level::level_enum level, const std::string& tag, const std::string& msg) { logger->log(level, "[{}] [{}]", tag, msg); }
	inline void debug(const std::string& tag, const std::string& msg) { log(spdlog::level::debug, tag, msg); }
	inline void info(const std::string& tag, const std::string& msg) { log(spdlog::level::info, tag, msg); }
	inline void error(const std::string& tag, const std::string& msg) { log(spdlog::level::err, tag, msg); }
	inline void warning(const std::string& tag, const std::string& msg) { log(spdlog::level::warn, tag, msg); }
	inline void fatal(const std::string& tag, const std::string& msg) { log(spdlog::level::critical, tag, msg); }
	inline void logger_drop() { spdlog::drop(logger_name); }
}

namespace cq::message {
	inline std::ostream& operator<<(std::ostream& out, const cq::Message& msg) { out << std::string(msg); return out; }
}
