#pragma once

#include "ctbx/stdafx.h"
#include "ctbx/logging/Logging.h"
#include "ctbx/types/Types.h"
#include "boost/asio.hpp"

namespace ctbx::cards {
	class Cards {
	private:
		static std::recursive_mutex _mtx;
		static std::shared_ptr<boost::asio::deadline_timer> _update_timer;
		static std::unordered_map<int64_t, std::unordered_map<int64_t, std::string>> _cards;
		static std::vector<ctbx::types::Group> _groups;
		static std::thread _update_thread;
	public:
		static Cards& get_cards();
		static void start_updating(const std::vector<ctbx::types::Group>&, long = 6 * 60 * 60);
		static void stop_updating();
		static void add_cards(int64_t, int64_t, const std::string&);
		static std::string get_card(int64_t, int64_t); // it should be noexcpt
		static void update_cards(int64_t);
		static void update_groups(const std::vector<ctbx::types::Group>&);
		static void update_groups(int64_t);
	private:
		Cards();
		static std::string _fetch_card(int64_t, int64_t);
	};
}