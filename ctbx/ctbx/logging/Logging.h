#pragma once

#include "ctbx/stdafx.h"

namespace ctbx::logging {
	const std::string logger_name = "logger";
	void logger_initialize(const std::string&, int = 0, int = 0);
	void log(const spdlog::level::level_enum, const std::string&, const std::string&);
	void info(const std::string&, const std::string&);
	void debug(const std::string&, const std::string&);
	void warning(const std::string&, const std::string&);
	void error(const std::string&, const std::string&);
	void fatal(const std::string&, const std::string&);
	void logger_drop();
}
