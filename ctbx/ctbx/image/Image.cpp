/// @file    Image.cpp
/// @brief   CTBX Image Utility
/// @author  wtdcode
/// @date    2018-03-21
/// @note    Image parse and convert
///

#include "./Image.h"

namespace logging = ctbx::logging;

using namespace logging::Image;
using namespace cq::utils;

namespace ctbx::image {

	/*
		受不了了
		迟早要重构
		2018/6/23

		啊，好菜的C++
		2018/6/27

		我写的是什么.jpg
		2018/9/14
	*/

	std::string Image::_image_root_path = "";
	std::map<std::string, std::string> Image::_tg_cache = {};
	std::map < std::string, std::string > Image::_cq_cache = {};
	const std::map <std::string, std::string> Image::_mimetype = { {".jpg", "image/jpeg"}, {".png", "image/png"}, {".gif", "image/gif"} };

	/// constructor
	/// @note Constructor of Image class
	/// 
	Image::Image()
		: _id(""), _md5(""), _is_valid(false), _file_path(""), _url(""), _suffix(""), _width(-1), _height(-1), _file_size(-1), _add_time(-1) {}

	/// constructor (by image name)
	/// @note  Constructor of Cards class
	/// @param img_name Image name
	/// 
	Image::Image(const std::string& img_name) {
		std::size_t pos = img_name.find_last_of(".");
		std::string md5 = img_name.substr(0, pos);
		std::string suffix = img_name.substr(pos);
		logging::debug(DEBUG_IMAGE_TAG, fmt::format(DEBUG_IMAGE_START_FETCHING_BY_MD5, md5));
		if (_cq_cache.count(md5)) {
			logging::debug(DEBUG_IMAGE_TAG, DEBUG_IMAGE_CACHED);
			_md5 = md5;
			_id = _cq_cache[md5];
			_is_valid = true;
			return;
		}
		if (_image_root_path == "")
			_get_root();
		std::fstream img(_image_root_path + md5 + suffix + ".cqimg", std::ios::in);
		if (img.is_open()) {
			_suffix = suffix;
			if (suffix == ".null") {
				logging::debug(DEBUG_IMAGE_TAG, DEBUG_IMAGE_CQ_FUCKING_BUG);
				_suffix = ".jpg";
			}
			logging::debug(DEBUG_IMAGE_TAG, fmt::format(DEBUG_IMAGE_SUFFIX, _suffix));
			_parse_cqimg(img);
		}
	}

	/// constructor (by md5 and id)
	/// @note  Constructor of Cards class
	/// @param md5 Image MD5
	/// @param id  Image id
	/// 
	Image::Image(const std::string& md5, const std::string& id) : Image() {
		_md5 = md5;
		_id = id;
		_is_valid = true;
		if (_image_root_path == "")
			_get_root();
	}

	/// constructor (from Telegram photo)
	/// @note  Constructor of Cards class
	/// @param p     Telegram photo pointer
	/// @param tgbot Telegram bot
	/// 
	Image::Image(const TgBot::PhotoSize::Ptr& p, const TgBot::Bot& tgbot) : Image() {
		_id = p->fileId;
		_file_size = p->fileSize;
		_height = p->height;
		_width = p->width;
		_is_valid = _get_suffix(tgbot);
		if (_image_root_path == "")
			_get_root();
	}

	/// constructor (from Telegram sticker)
	/// @note  Constructor of Cards class
	/// @param p     Telegram sticker pointer
	/// @param tgbot Telegram bot
	/// 
	Image::Image(const TgBot::Sticker::Ptr& p, const TgBot::Bot& tgbot) {
		_id = p->fileId;
		_file_size = p->fileSize;
		_height = p->height;
		_width = p->width;
		_is_valid = _get_suffix(tgbot);
		if (_image_root_path == "")
			_get_root();
	}

	/// _parse_cqimg
	/// @note   Parse CQImg file to image
	/// @param  in   CQImg file stream    
	/// @return void
	/// 
	void Image::_parse_cqimg(std::istream& in) {
		std::string buffer;
		for (int i = 0; i < 7; i++) {
			std::getline(in, buffer);
			if (i == 0 && buffer == "[image]")
				continue;
			else if (i == 0 && buffer != "[image]") {
				logging::warning(WARNING_CQIMG_PARSING_TAG, WARNING_CQIMG_MAGIC_ERROR);
				_is_valid = false;
				return;
			}
			else {
				std::size_t pos = buffer.find_first_of("=");
				if (pos == std::string::npos) {
					logging::warning(WARNING_CQIMG_PARSING_TAG, WARNING_CQIMG_PROPERTY_ERROR);
					_is_valid = false;
					return;
				}
				std::string left = buffer.substr(0, pos);
				std::string right = buffer.substr(pos + 1);
				if (left == "md5")
					_md5 = right;
				else if (left == "width")
					_width = std::stoi(right);
				else if (left == "height")
					_height = std::stoi(right);
				else if (left == "size")
					_file_size = std::stoi(right);
				else if (left == "url")
					_url = right;
				else if (left == "addtime")
					_add_time = std::stoi(right);
			}
		}
		_is_valid = true;
		return;
	}

	/// send_to_tg
	/// @note   Send image to Telegram 
	/// @param  chat_id Telegram group id
	/// @param  tgbot   Telegram bot
	/// @param  caption Picture caption
	/// @param  from    Where this picture from
	/// @return Boolean value of sending status
	/// 
	bool Image::send_to_tg(const int64_t chat_id, const TgBot::Bot& tgbot, const std::string& caption, const types::SOFTWARE_TYPE from){
		logging::info(INFO_IMAGE_TAG, fmt::format(INFO_IMAGE_SEND_TO_TG, _md5));
		if (!_is_valid) {
			logging::warning(WARNING_IMAGE_SENDING_TAG, fmt::format(WARNING_IMAGE_NOT_VALID, _md5, _id));
			return false;
		}
		auto send_impl = [&](const std::string& id) {
			try {
				tgbot.getApi().sendPhoto(chat_id, id, caption);
			}
			catch (const TgBot::TgException& e) {
				try {
					tgbot.getApi().sendDocument(chat_id, id, caption);
				}
				catch (const std::exception& e) {
					logging::debug(DEBUG_IMAGE_TAG, fmt::format(DEBUG_IMAGE_SENDING_FAILED_ID, id, string_decode(std::string(e.what()), Encoding::ANSI)));
				}
			}
			catch (const std::exception& e) {
				logging::debug(DEBUG_IMAGE_TAG, fmt::format(DEBUG_IMAGE_SENDING_FAILED_ID, id, string_decode(std::string(e.what()), Encoding::ANSI)));
			}
		};
		if (_cq_cache.count(_md5)) {
			try {
				send_impl(_cq_cache.at(_md5));
				return true;
			}
			catch (const std::exception& e) {
				logging::warning(WARNING_IMAGE_SENDING_TAG, fmt::format(WARNING_IMAGE_CACHED_SENDING_FALIED, _md5, string_decode(std::string(e.what()), Encoding::ANSI)));
				_cq_cache.erase(_cq_cache.find(_md5));
				return false;
			}
		}
		else if (from == types::SOFTWARE_TYPE::TG && !_id.empty()) {
			try {
				send_impl(_id);
				return true;
			}
			catch (const std::exception& e) {
				logging::debug(DEBUG_IMAGE_TAG, fmt::format(DEBUG_IMAGE_SENDING_FAILED_ID, _id, string_decode(std::string(e.what()), Encoding::ANSI)));
				return false;
			}
		}
		else {
			if (!_download(tgbot))
				return false;
			try {
				TgBot::Message::Ptr new_message;
				if (_suffix != ".gif") {
					new_message = tgbot.getApi().sendPhoto(chat_id,
						TgBot::InputFile::fromFile(_file_path, _mimetype.at(_suffix)), caption);
					_cq_cache[_md5] = (*(new_message->photo.rbegin()))->fileId;
				}
				else {
					new_message = tgbot.getApi().sendDocument(chat_id,
						TgBot::InputFile::fromFile(_file_path, _mimetype.at(_suffix)), caption);
					_cq_cache[_md5] = new_message->document->fileId;
				}
				return true;
			}
			catch (const TgBot::TgException& e) {
				logging::debug(DEBUG_IMAGE_TAG,fmt::format(DEBUG_IMAGE_SENDING_FAILED_MD5, _md5, string_decode(std::string(e.what()), Encoding::ANSI)));
				return false;
			}
			catch (const std::out_of_range& e) {
				logging::warning(WARNING_IMAGE_SENDING_TAG, fmt::format(WARNING_IMAGE_FAIL_TO_GET_ID, string_decode(std::string(e.what()), Encoding::ANSI)));
				return false;
			}
			catch (const std::exception& e) {
				logging::debug(DEBUG_IMAGE_TAG, fmt::format(DEBUG_IMAGE_SENDING_FAILED_MD5, _md5, string_decode(std::string(e.what()), Encoding::ANSI)));
				return false;
			}
		}
	}

	/// send_to_qq
	/// @note   Send image to QQ 
	/// @param  group_id QQ group id
	/// @param  tgbot    Telegram bot
	/// @param  caption  Picture caption
	/// @param  from     Where this picture from
	/// @return Boolean  value of sending status
	/// 
	bool Image::send_to_qq(const int64_t group_id, const TgBot::Bot& tgbot, const std::string& caption, const types::SOFTWARE_TYPE from){
		// TODO : 对于QQ可以考虑在一条消息内发送多个图片
		logging::info(INFO_IMAGE_TAG, fmt::format(INFO_IMAGE_SEND_TO_CQ, _id));
		if (!_is_valid) {
			logging::warning(WARNING_IMAGE_SENDING_TAG, fmt::format(WARNING_IMAGE_NOT_VALID, _md5, _id));
			return false;
		}
		std::string file_name;
		if (from == types::SOFTWARE_TYPE::QQ) {
			file_name = _md5 + _suffix;
		}
		else if (!_download(tgbot)) {
			return false;
		}
		else {
			file_name = _id + _suffix;
		}
		cq::MessageSegment img_seg = cq::MessageSegment::image(file_name);
		cq::Message img_msg;
		if (!caption.empty())
			img_msg += caption;
		img_msg += img_seg;
		try {
			cq::api::send_group_msg(group_id, img_msg);
		}
		catch (const cq::exception::ApiError& e) {
			logging::debug(DEBUG_IMAGE_TAG,fmt::format(DEBUG_IMAGE_SENDING_FAILED_ID_PATH, _id, _file_path, string_decode(std::string(e.what()), Encoding::ANSI)));
			return false;
		}
		return true;
	}

	/// send
	/// @note   Send image to QQ 
	/// @param  group   Group info
	/// @param  tgbot   Telegram bot
	/// @param  caption Picture caption
	/// @param  from    Where this picture from
	/// @return Boolean value of sending status
	/// 
	bool Image::send(const ctbx::types::Group& group, const TgBot::Bot& tgbot, const std::string& caption="", const types::SOFTWARE_TYPE from = types::SOFTWARE_TYPE::TG){
		if (group.type == ctbx::types::SOFTWARE_TYPE::QQ)
			return send_to_qq(group.group_id, tgbot, caption, from);
		else
			return send_to_tg(group.group_id, tgbot, caption, from);
	}

	/// get_md5
	/// @note   get md5 of image
	/// @return md5 value
	std::string Image::get_md5() { return _md5; }
	
	/// get_id
	/// @note   get image id
	/// @return image id
	std::string Image::get_id() { return _id; }

	/// _get_root
	/// @note   Log image absolute path
	/// @return void
	void Image::_get_root() {
		logging::debug(DEBUG_IMAGE_TAG, DEBUG_IMAGE_GET_ROOT);
		_image_root_path = cq::utils::ansi(cq::dir::root());
		_image_root_path += "data\\image\\";
		logging::debug(DEBUG_IMAGE_TAG,fmt::format(DEBUG_IMAGE_ROOT, _image_root_path));
	}

	/// _download
	/// @note   Download image
	/// @param  tgbot Telegram bot
	/// @return Boolean status of download status
	bool Image::_download(const TgBot::Bot& tgbot) {
		if (!_is_valid)
			return false;
		if (!_md5.empty() && !_id.empty())
			return false;
		std::string img_content;
		if (_md5.empty()) {
			logging::debug(DEBUG_IMAGE_TAG, fmt::format(DEBUG_DOWNLOAD_TG_IMAGE, _id));
			try {
				std::shared_ptr<TgBot::File> img_file = tgbot.getApi().getFile(_id);
				img_content = tgbot.getApi().downloadFile(img_file->filePath);
				_file_path = _image_root_path + _id + _suffix;
				logging::debug(DEBUG_IMAGE_TAG, fmt::format(DEBUG_DOWNLOAD_FULL_IMAGE_PATH, _file_path));
				if (_suffix == ".webp") {
					_is_valid = _convert_webp(img_content);
					return _is_valid;
				}
			}
			catch (const std::exception& e) {
				logging::error(DEBUG_IMAGE_TAG, fmt::format(DEBUG_DOWNLOAD_TG_IMAGE_FAILED,  _id, string_decode(std::string(e.what()), Encoding::ANSI)));
				_is_valid = false;
				return false;
			}
		}
		else if (_id.empty()) {
			logging::debug(DEBUG_IMAGE_TAG, fmt::format(DEBUG_DOWNLOAD_CQ_IMAGE, _md5));
			try {
				img_content = TgBot::HttpClient::getInstance().makeRequest(_url, std::vector<TgBot::HttpReqArg>());
				_file_path = _image_root_path + _md5 + _suffix;
				logging::debug(DEBUG_IMAGE_TAG, fmt::format(DEBUG_DOWNLOAD_FULL_IMAGE_PATH, _file_path));
			}
			catch (const std::exception& e) {
				logging::debug(DEBUG_IMAGE_TAG, fmt::format(DEBUG_DOWNLOAD_CQ_IMAGE_FAILED, _md5, string_decode(std::string(e.what()), Encoding::ANSI)));
				_is_valid = false;
				return false;
			}
		}
		std::fstream img(_file_path, std::ios::out | std::ios::binary | std::ios::trunc);
		img << img_content;
		return true;
	}

	/// _convert_webp
	/// @note   Convert image to webp format
	/// @param  img_content Content of image
	/// @return Boolean status of converting status
	bool Image::_convert_webp(const std::string& img_content) {
		std::string tmp_dir = cq::utils::ansi(cq::dir::app_tmp());
		_file_path = tmp_dir + _id + _suffix;
		std::fstream img(_file_path, std::ios::out | std::ios::binary | std::ios::trunc);
		img << img_content;
		img.close();
		const uint8_t* data = nullptr;
		size_t data_size;
		WebPDecoderConfig config;
		WebPDecBuffer* const output_buffer = &config.output;
		WebPBitstreamFeatures* const bitstream = &config.input;
		auto free_res = [&]() {
			WebPFreeDecBuffer(output_buffer);
			free((void*)data);
		};
		if (!WebPInitDecoderConfig(&config)) {
			logging::warning(WARNING_CONVERT_WEBP_TAG, WARNING_CONFIG_INITIALIZE_FAILED);
			return false;
		}
		if (!LoadWebP(_file_path.c_str(), &data, &data_size, bitstream)) {
			logging::warning(WARNING_CONVERT_WEBP_TAG, fmt::format(WARNING_FILE_NOT_FOUND, _file_path));
			return false;
		}
		output_buffer->colorspace = bitstream->has_alpha ? MODE_RGBA : MODE_RGB;
		if (DecodeWebP(data, data_size, &config) != VP8_STATUS_OK) {
			logging::warning(WARNING_CONVERT_WEBP_TAG, WARNING_DECODE_ERROR);
			return false;
		}
		_suffix = ".png";
		_file_path = _image_root_path + _id + _suffix;
		WebPSaveImage(output_buffer, WebPOutputFileFormat::PNG, _file_path.c_str());
		free_res();
		return true;
	}

	/// _get_suffix
	/// @note   Get suffix of image file (file format)
	/// @param  tgbot Telegram bot
	/// @return Boolean status of retriving status
	bool Image::_get_suffix(const TgBot::Bot& tgbot) {
		try {
			std::shared_ptr<TgBot::File> img_file = tgbot.getApi().getFile(_id);
			logging::debug(DEBUG_IMAGE_TAG, fmt::format(DEBUG_IMAGE_GET_ID, img_file->fileId));
			logging::debug(DEBUG_IMAGE_TAG, fmt::format(DEBUG_IMAGE_GET_ID, img_file->filePath));
			_suffix = img_file->filePath.substr(img_file->filePath.find_last_of("."));
		}
		catch (const std::exception& e) {
			logging::error(DEBUG_IMAGE_TAG, fmt::format(DEBUG_DOWNLOAD_TG_IMAGE_FAILED, _id, string_decode(std::string(e.what()), Encoding::ANSI)));
			return false;
		}
		return true;
	}
}