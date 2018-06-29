#pragma once

#include "ctbx/stdafx.h"
#include "ctbx/types/Types.h"
#include "ctbx/logging/Logging.h"
#include "tgbot/net/HttpClient.h"
#include "./imageio/image_enc.h"
#include "./imageio/webpdec.h"

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
		Image(const TgBot::Sticker::Ptr&, const TgBot::Bot&);
		bool send_to_tg(const int64_t, const TgBot::Bot&, const std::string&, const ctbx::types::SOFTWARE_TYPE);
		bool send_to_qq(const int64_t , const TgBot::Bot&, const std::string&, const ctbx::types::SOFTWARE_TYPE);
		bool send(const ctbx::types::Group&, const TgBot::Bot&, const std::string&, const ctbx::types::SOFTWARE_TYPE);
		std::string get_md5();
		std::string get_id();
	private:
		Image();
		static void _get_root();
		void _parse_cqimg(std::istream&);
		bool _download(const TgBot::Bot&);
		bool _convert_webp(const std::string&);
		bool _get_suffix(const TgBot::Bot&);
	};
}