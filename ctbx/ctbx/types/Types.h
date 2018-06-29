#pragma once

#include "ctbx/stdafx.h"

namespace ctbx::types {
	enum SOFTWARE_TYPE {
		QQ = 0,
		TG = 1
	};

	typedef struct _GROUP {
		SOFTWARE_TYPE type;
		int64_t group_id;
		bool operator<(const _GROUP& a)const { return group_id < a.group_id; } // just make STL happy. :p
	} Group, *PGroup;

	typedef struct _USER {
		SOFTWARE_TYPE type;
		std::int64_t id;
		std::string card;
		bool operator<(const _USER& a)const { return id < a.id; } // make STL happy, too. :D
	} User, *PUser;


}
