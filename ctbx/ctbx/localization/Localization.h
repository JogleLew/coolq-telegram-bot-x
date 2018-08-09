#pragma once

#include "ctbx/stdafx.h"

namespace ctbx::localization {
	void locale_initialize();
	void set_language(const std::string&);
	std::string str(const std::string& key);
}