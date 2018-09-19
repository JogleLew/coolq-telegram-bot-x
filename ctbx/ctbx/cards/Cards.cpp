/// @file    Cards.cpp
/// @brief   CTBX Group Member Cards
/// @author  wtdcode
/// @date    2018-03-21
/// @note    Group member card management
///

#include "./Cards.h"

namespace logging = ctbx::logging;
namespace types   = ctbx::types;

using namespace logging::cards;
using namespace cq::utils;

namespace ctbx::cards {
	
	typedef std::function<void(const boost::system::error_code&)> timer_callback;

	std::recursive_mutex Cards::_mtx;
	std::shared_ptr<boost::asio::deadline_timer> Cards::_update_timer;
	std::thread Cards::_update_thread;
	std::unordered_map<int64_t, std::unordered_map<int64_t, std::string>> Cards::_cards;
	std::vector<ctbx::types::Group> Cards::_groups;

	/// constructor
	/// @note Constructor of Cards class
	/// 
	Cards::Cards() {

	}

	/// get_cards
	/// @note   Get static Cards object
	/// @return Cards object
	/// 
	Cards& Cards::get_cards(){
		static Cards cards;
		return cards;
	}

	/// start_updating
	/// @note   Start a thread to update member cards
	/// @param  groups  Target group 
	/// @param  seconds Updating interval (in second)
	/// @return void
	/// 
	void Cards::start_updating(const std::vector<ctbx::types::Group>& groups, long seconds){
		if (_update_timer.use_count()) {
			logging::warning(WARNING_TIMER_STARTED_TAG, WARNING_TIMER_STARTED);
			return;
		}
		_update_thread = std::move(std::thread([seconds]() {
			boost::asio::io_context io;
			auto timer_deleter = [](boost::asio::deadline_timer* p) {
				p->cancel();
				delete p;
				logging::debug(DEBUG_TIMER_TAG, DEBUG_TIMER_STOPPED);
			};
			timer_callback update_all_cards = [&](const boost::system::error_code& err) {
				if (!err) {
					_mtx.lock();
					for (auto &it : _groups)
						if (it.type == types::SOFTWARE_TYPE::QQ)
							update_cards(it.group_id);
					_mtx.unlock();
					_update_timer->expires_at(_update_timer->expires_at() + boost::posix_time::seconds(seconds));
					_update_timer->async_wait(update_all_cards);
				}
			};
			// hard-coded interval
			// TODO: configurable
			_update_timer = std::shared_ptr<boost::asio::deadline_timer>(new boost::asio::deadline_timer(io, boost::posix_time::hours(6)), timer_deleter);
			logging::info(INFO_TIMER_STARTING_TAG, fmt::format(INFO_TIMER_STARTING, seconds));
			_update_timer->async_wait(update_all_cards);
			io.run();
		}));
	}

	/// stop_updating
	/// @note   Stop the thread of updating member cards
	/// @return void
	/// 
	void Cards::stop_updating() { 
		if(_update_timer.use_count())
			_update_timer.reset();
		if (_update_thread.joinable())
			_update_thread.join();
	}

	/// add_cards
	/// @note   Add a member card item
	/// @param  group_id Group id 
	/// @param  user_id  Group member id
	/// @param  card     Group card name
	/// @return void
	/// 
	void Cards::add_cards(int64_t group_id, int64_t user_id, const std::string& card) {
		_mtx.lock();
		_cards[group_id][user_id] = card;
		_mtx.unlock();
	}

	/// get_card
	/// @note   Get member card by group and user id
	/// @param  group_id Group id 
	/// @param  user_id  Group member id
	/// @return Group card name
	///
	std::string Cards::get_card(int64_t group_id, int64_t user_id){
		_mtx.lock();
		if (!_cards.count(group_id) || !_cards[group_id].count(user_id)) {
			_mtx.unlock();
			return _fetch_card(group_id, user_id);
		}
		std::string card = _cards[group_id][user_id];
		_mtx.unlock();
		return card;
	}

	/// update_cards
	/// @note   Function of batch updating member card
	/// @param  group_id Group id 
	/// @return void
	///
	void Cards::update_cards(int64_t group_id){
		logging::info(INFO_UPDATE_TAG, fmt::format(INFO_UPDATE_CARDS, group_id));
		std::vector<cq::GroupMember> memlist;
		try {
			memlist = cq::api::get_group_member_list(group_id);
		}
		catch (const std::exception&e) {
			logging::debug(DEBUG_UPDATE_TAG, fmt::format(DEBUG_UPDATE_ERROR, group_id, string_decode(std::string(e.what()), Encoding::ANSI)));
			return;
		}
		for (cq::GroupMember it : memlist) {
			if (it.card.compare(std::to_string("")) == 0)
				add_cards(group_id, it.user_id, it.card);
			else
				add_cards(group_id, it.user_id, it.nickname);
		}
		logging::info(INFO_UPDATE_TAG,fmt::format(INFO_UPDATED, group_id));
	}

	/// update_groups
	/// @note   Add a list of groups to target updating list
	/// @param  groups list of Group id 
	/// @return void
	///
	void Cards::update_groups(const std::vector<types::Group>& groups) { 
		_mtx.lock();
		_groups.assign(groups.begin(), groups.end()); 
		_mtx.unlock();
	}

	/// update_groups
	/// @note   Add a group to target updating list
	/// @param  group_id Group id 
	/// @return void
	///
	void Cards::update_groups(int64_t group_id) {
		_mtx.lock();
		_groups.push_back(types::Group({ types::SOFTWARE_TYPE::QQ, group_id }));
		_mtx.unlock();
	}

	/// _fetch_card
	/// @note   fetch one group member's member card
	/// @param  group_id Group id 
	/// @param  user_id Group member id 
	/// @return void
	///
	std::string Cards::_fetch_card(int64_t group_id, int64_t user_id){
		try {
			logging::debug(DEBUG_UPDATE_TAG, fmt::format(DBUEG_FETCH_CARD, group_id, user_id));
			cq::GroupMember member = cq::api::get_group_member_info(group_id, user_id, true);
			std::string card = (member.card == "" ? member.nickname : member.card);
			add_cards(group_id, user_id, card);
			return card;
		}
		catch (const std::exception&e) {
			logging::error(DEBUG_UPDATE_TAG, fmt::format(DEBUG_FETCH_CARD_ERROR, string_decode(std::string(e.what()), Encoding::ANSI)));
			return std::to_string(user_id);
		}
	}
}