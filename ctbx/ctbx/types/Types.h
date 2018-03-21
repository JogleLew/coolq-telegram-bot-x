#pragma once

#include "ctbx/stdafx.h"

namespace ctbx::types {
	enum GROUP_TYPE {
		QQ = 0,
		TG = 1
	};
	typedef struct _GROUP {
		GROUP_TYPE type;
		int64_t group_id;
		bool operator<(const _GROUP& a)const { return group_id < a.group_id; } // just make STL happy. :p
	} Group, *PGroup;
	typedef struct _USER {
		GROUP_TYPE type;
		std::int64_t id;
		std::string card;
	} User, *PUser;
	typedef struct _PHOTO {
		GROUP_TYPE type;
		std::int64_t id;
		std::int32_t width;
		std::int32_t height;
		std::int32_t filesize;
	} Photo, *PPhoto;
}