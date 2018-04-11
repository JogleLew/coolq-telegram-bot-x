#pragma once

#include "ctbx/stdafx.h"
#include "ctbx/types/Types.h"
#include "ctbx/logging/Logging.h"
#include "tgbot/net/HttpClient.h"

namespace ctbx::image {
	class Image {
	private:
		static const std::map < std::string, std::string > _mimetype;
		static std::string _image_root_path;
		static std::map<std::string, std::string> _cq_cache;
		static std::map <std::string , std::string > _tg_cache; // maybe used in the future
		std::string _id;
		std::string _md5;
		bool _is_valid;
		std::string _file_path;
		std::string _url;
		std::string _suffix;
		std::int32_t _width;
		std::int32_t _height;
		std::int32_t _file_size;
		std::int32_t _add_time;
	public:
		Image(const std::string&);
		Image(const std::string&, const std::string&);
		Image(const TgBot::PhotoSize::Ptr&, const TgBot::Bot&);

		void send_to_tg(const int64_t, const TgBot::Bot&, const std::string&);
		void send_to_qq(const int64_t , const TgBot::Bot&);
		void send(const ctbx::types::Group&, const TgBot::Bot&);
		
	private:
		Image();
		static void _get_root();
		void _parse_cqimg(std::istream&);
		void _download(const TgBot::Bot&);
	};
}