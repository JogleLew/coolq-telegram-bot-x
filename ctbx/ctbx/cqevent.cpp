#include "./ctbx.h"

#include <memory>
#include <thread>
#include <unordered_map>
#include <stdint.h>
#include <string>
#include <mutex>
#include <chrono>

namespace logging = cq::logging;
namespace exception = cq::exception;
namespace tgevent = ctbx::tgevent;
namespace cqevent = ctbx::cqevent;
namespace api = cq::api;

static std::shared_ptr<TgBot::Bot> tgbot(static_cast<TgBot::Bot*>(0));
static bool polling = true;
static std::thread tgbot_polling_thread;
static std::unordered_map<int64_t, std::string> cards;
static std::mutex cards_mtx;

namespace ctbx::cqevent {

	void update_cards() {
		cards_mtx.lock();
		logging::info(u8"QQBot", u8"开始更新群" + std::to_string(QQ_GROUP) + "名片");
		cards.clear();
		const auto memlist = api::get_group_member_list(QQ_GROUP);
		for (cq::GroupMember it : memlist) {
			if (it.card.compare(std::to_string("")) != 0)
				cards[it.user_id] = it.card;
		}
		logging::info(u8"QQBot", u8"群名片已经更新");
		cards_mtx.unlock();
	}

	void cards_start_updating() {
		logging::debug(u8"QQBot", u8"启动群名片自动更新线程");
		std::thread update_thread([]() {
			while (true) {
				update_cards();
				std::this_thread::sleep_for(std::chrono::hours(4));
			}
		});
		update_thread.detach();
	}

	void bot_start_polling() {
		polling = true;
		std::thread temp_thread([]() {
			try {
				logging::debug(u8"TGBot", u8"Bot开始polling");
				TgBot::TgLongPoll longpoll(*tgbot);
				while (polling)
					longpoll.start();
			}
			catch (TgBot::TgException& e) {
				logging::debug(u8"TG", u8"TgBot错误，错误信息" + std::string(e.what()));
			}
		});
		tgbot_polling_thread = std::move(temp_thread);
	}

	void bot_on_enable() {
		logging::info(u8"Bot", u8"Bot开始初始化");
		logging::debug(u8"TGBot", u8"开始初始化TGBot，根据网络情况需要的时间有较大区别");
		logging::debug(u8"TGBot", u8"TGBot的Token为:" + std::to_string(BOT_TOKEN));
		std::shared_ptr<TgBot::Bot> temp_ptr(new TgBot::Bot(BOT_TOKEN));
		tgbot = temp_ptr;
		tgbot->getEvents().onAnyMessage(tgevent::bot_receive_anymessage);
		logging::debug(u8"TGBot", u8"TGBot初始化完成，TGBot用户名:" + tgbot->getApi().getMe()->username);
		cards_start_updating();
		bot_start_polling();
		logging::info(u8"Bot", u8"初始化完毕");
	}

	void bot_on_disable() {
		logging::debug(u8"Bot", u8"Bot即将退出，停止TGBot，这可能需要一段时间");
		polling = false;
		if(tgbot_polling_thread.joinable())
			tgbot_polling_thread.join();
		tgbot.reset();
	}

	void bot_on_exit() {
		bot_on_disable();
	}
	void bot_receive_groupmessage(const cq::event::GroupMessageEvent& e) {
		if (e.group_id != QQ_GROUP)
			return;
		logging::info(u8"QQBot", u8"收到QQ信息:" + e.message + u8"，开始转发");
		cards_mtx.lock();
		std::string user_card="";
		if (cards.count(e.user_id))
			user_card = cards[e.user_id];
		else {
			cq::GroupMember user = api::get_group_member_info(e.group_id, e.user_id);
			user_card = user.card;
		}
		cards_mtx.unlock();
		logging::debug(u8"QQBot", u8"获得群名片为:" + user_card);
		try {
			tgbot->getApi().sendMessage(TG_GROUP, user_card + " : " + e.message);
		}
		catch (TgBot::TgException& e) {
			logging::debug(u8"QQBot", u8"QQ->Tg转发失败，原因:" + std::to_string(e.what()));
		}
	}
}