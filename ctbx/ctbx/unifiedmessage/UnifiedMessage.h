#pragma once
#include "ctbx/stdafx.h"
#include "ctbx/types/Types.h"
#include "ctbx/logging/Logging.h"
#include "ctbx/cards/Cards.h"
#include "ctbx/image/Image.h"

namespace ctbx::message {
	enum MSG_TYPE {
		Invalid,
		Image,
		Forward,
		Reply,
		Plain
	};

	typedef ctbx::image::Image UImage;
	typedef struct _REPLY {
		ctbx::types::SOFTWARE_TYPE type;
		std::int64_t id;
		std::string card;
		// 和ctbx::types::User定义一致，为了以后方便扩展
	} UReply;
	typedef ctbx::types::User UForward;
	typedef std::string UPlain;

	class UnifiedMessage {
	private:
		typedef struct _MessageSegment {
			MSG_TYPE type;
			std::shared_ptr<UImage> image;
			std::shared_ptr<UReply> reply;
			std::shared_ptr<UForward> forward;
			std::shared_ptr<UPlain> plain;
			_MessageSegment(MSG_TYPE t, UImage* pimage, UReply* preply, UForward* pforward, UPlain* pplain) :
				type(t), image(pimage), reply(preply), forward(pforward), plain(pplain) {}
			_MessageSegment(UImage* p) : _MessageSegment(MSG_TYPE::Image, p, nullptr, nullptr, nullptr) {}
			_MessageSegment(UReply* p) : _MessageSegment(MSG_TYPE::Reply, nullptr, p, nullptr, nullptr) {}
			_MessageSegment(UForward* p) : _MessageSegment(MSG_TYPE::Forward, nullptr, nullptr, p, nullptr) {}
			_MessageSegment(UPlain* p) : _MessageSegment(MSG_TYPE::Plain, nullptr, nullptr, nullptr, p) {}
		} *PMessageSegment, MessageSegment;

		std::list<MessageSegment> _segs;
		std::string _unified_card;
		ctbx::types::User _from;
		short _image_count;
		bool _image_only;
	public:
		explicit UnifiedMessage(const cq::GroupMessageEvent&);
		explicit UnifiedMessage(const TgBot::Message::Ptr&, const TgBot::Bot&);
		void send(const ctbx::types::Group&, const TgBot::Bot&, ctbx::types::SOFTWARE_TYPE);
		void send_to_qq(const int64_t, const TgBot::Bot&, ctbx::types::SOFTWARE_TYPE);
		void send_to_tg(const int64_t, const TgBot::Bot&, ctbx::types::SOFTWARE_TYPE);
	private:
		std::string _parse_card();
		std::string _preprocess_rich_text(const TgBot::Message::Ptr&);
		void _debug_remaining_msg(const cq::Message&);
		void _debug_all_segs();
	};
}