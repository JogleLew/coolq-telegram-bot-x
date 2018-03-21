#pragma once

#include<string>
#include<stdint.h>
#include<map>
#include<unordered_map>
#include<vector>
#include<mutex>

#include "spdlog/spdlog.h"
#include "json/json.h"

// split to two files?

namespace ctbx::utils{
	enum GROUP_TYPE {
		QQ = 0,
		TG = 1
	};
	typedef struct _GROUP {
		GROUP_TYPE type;
		int64_t group_id;
		bool operator<(const _GROUP& a)const { return group_id < a.group_id; } // just make STL happy. :p
	} Group, *PGroup;
}
