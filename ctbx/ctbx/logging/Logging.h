#pragma once

#include "ctbx/stdafx.h"
#include "Locale.h"

#define LOGGER_NAME "ctbx_logger"

namespace ctbx::logging {
	inline void logger_initialize(const std::string& app_dir, int hour = 0, int minute = 0) {
		auto logger = spdlog::daily_logger_mt(LOGGER_NAME, app_dir, hour, minute);
		spdlog::set_default_logger(logger);
		spdlog::set_pattern("[%Y-%m-%d %H:%M:%S] [%l] %v");
		spdlog::flush_on(spdlog::level::level_enum::trace);
		spdlog::set_level(spdlog::level::level_enum::debug);
	}
	
	inline void log(const spdlog::level::level_enum level, const std::string& tag, const std::string& msg) { spdlog::log(level, "[{}] [{}]", tag, msg); }
	inline void debug(const std::string& tag, const std::string& msg) { log(spdlog::level::debug, tag, msg); }
	inline void info(const std::string& tag, const std::string& msg) { log(spdlog::level::info, tag, msg); }
	inline void error(const std::string& tag, const std::string& msg) { log(spdlog::level::err, tag, msg); }
	inline void warning(const std::string& tag, const std::string& msg) { log(spdlog::level::warn, tag, msg); }
	inline void fatal(const std::string& tag, const std::string& msg) { log(spdlog::level::critical, tag, msg); }
	inline void logger_drop() { spdlog::drop(LOGGER_NAME); }
}

namespace cq::message {
	inline std::ostream& operator<<(std::ostream& out, const cq::Message& msg) { out << std::string(msg); return out; }
}
