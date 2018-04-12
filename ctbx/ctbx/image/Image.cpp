#include "./Image.h"

namespace logging = ctbx::logging;

namespace ctbx::image {

	std::string Image::_image_root_path = "";
	std::map<std::string, std::string> Image::_tg_cache = {};
	std::map < std::string, std::string > Image::_cq_cache = {};
	const std::map <std::string, std::string> Image::_mimetype = { {".jpg", "image/jpeg"}, {".png", "image/png"}, {".gif", "image/gif"} };

	Image::Image()
		: _id(""), _md5(""), _is_valid(false), _file_path(""), _url(""), _suffix(""), _width(-1), _height(-1), _file_size(-1), _add_time(-1) {}

	Image::Image(const std::string& img_name) {
		std::size_t pos = img_name.find_last_of(".");
		std::string md5 = img_name.substr(0, pos);
		std::string suffix = img_name.substr(pos);
		logging::debug(u8"Image", u8"开始获取md5为" + md5 + u8"的图片");
		if (_cq_cache.count(md5)) {
			logging::debug(u8"Image", u8"缓存中存在该图片！");
			return;
		}
		if (_image_root_path == "")
			_get_root();
		std::fstream img(_image_root_path + md5 + suffix + ".cqimg", std::ios::in);
		if (img.is_open()) {
			_suffix = suffix;
			if (suffix == ".null") {
				logging::debug(u8"Image", u8"收到null后缀图片，强行修改为jpg");
				_suffix = ".jpg";
			}
			logging::debug(u8"Image", u8"后缀：" + _suffix + "有效，开始读取");
			_parse_cqimg(img);
		}
	}
	Image::Image(const std::string& md5, const std::string& id) : Image() {
		_md5 = md5;
		_id = id;
		_is_valid = true;
	}

	Image::Image(const TgBot::PhotoSize::Ptr& p, const TgBot::Bot& tgbot) : Image() {
		_id = p->fileId;
		_file_size = p->fileSize;
		_height = p->height;
		_width = p->width;
		_is_valid = _get_suffix(tgbot);
	}

	Image::Image(const TgBot::Sticker::Ptr& p, const TgBot::Bot& tgbot) {
		_id = p->fileId;
		_file_size = p->fileSize;
		_height = p->height;
		_width = p->width;
		_is_valid = _get_suffix(tgbot);
	}

	void Image::_parse_cqimg(std::istream& in) {
		std::string buffer;
		for (int i = 0; i < 7; i++) {
			std::getline(in, buffer);
			if (i == 0 && buffer == "[image]")
				continue;
			else if (i == 0 && buffer != "[image]") {
				logging::warning(u8"Image", u8"Magic有误，无法读取");
				_is_valid = false;
				return;
			}
			else {
				std::size_t pos = buffer.find_first_of("=");
				if (pos == std::string::npos) {
					logging::warning(u8"Image", u8"属性无法读取");
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

	void Image::send_to_tg(const int64_t chat_id, const TgBot::Bot& tgbot, const std::string& caption=""){
		logging::info(u8"Image", u8"开始发送md5为" + _md5 + "的图片");
		if (!_is_valid) {
			logging::warning(u8"Image", u8"无效的图片发送！md5=" + _md5 + " _id:" + _id);
			return;
		}
		if (_cq_cache.count(_md5)) {
			tgbot.getApi().sendPhoto(chat_id, _cq_cache[_md5], caption);
			return;
		}
		_download(tgbot);
		try {
			TgBot::Message::Ptr new_message = tgbot.getApi().sendPhoto(chat_id, 
																	   TgBot::InputFile::fromFile(_file_path, _mimetype.at(_suffix)), 
																	   caption);
			_cq_cache[_md5] = new_message->photo[0]->fileId;
		}
		catch (const TgBot::TgException& e) {
			logging::error(u8"Image", u8"发送md5为" + _md5 + "的图片失败，原因：" + std::string(e.what()));
		}
		catch (const std::out_of_range& e) {
			logging::error(u8"Image", u8"返回的图片id获取失败！原因:" + std::string(e.what()));
		}
	}

	void Image::send_to_qq(const int64_t group_id, const TgBot::Bot& tgbot){
		logging::info(u8"Image", u8"开始发送id为" + _id + "的图片");
		if (!_is_valid) {
			logging::warning(u8"Image", u8"无效的图片发送！md5=" + _md5 + " _id:" + _id);
			return;
		}
		_download(tgbot);
		cq::MessageSegment img_seg = cq::MessageSegment::image(_file_path);
		cq::Message img_msg = img_seg;
		try {
			cq::api::send_group_msg(group_id, img_msg);
		}
		catch (const cq::exception::ApiError& e) {
			logging::error(u8"Image", u8"发送id为" + _id + "路径为" + _file_path + "的图片失败，原因" + std::string(e.what()));
		}
		return;
	}

	void Image::send(const ctbx::types::Group& group, const TgBot::Bot& tgbot){
		if (group.type == ctbx::types::GROUP_TYPE::QQ)
			send_to_qq(group.group_id, tgbot);
		else
			send_to_tg(group.group_id, tgbot);
	}

	void Image::_get_root() {
		logging::debug(u8"Image", u8"开始获取图片目录");
		_image_root_path = cq::dir::root();
		_image_root_path += "data\\image\\";
		logging::debug(u8"Image", u8"图片目录为:" + _image_root_path);
	}

	void Image::_download(const TgBot::Bot& tgbot) {
		if (!_is_valid)
			return;
		if (!_md5.empty() && !_id.empty())
			return;
		std::string img_content;
		if (_md5.empty()) {
			logging::debug(u8"Image", u8"开始下载id为" + _id + "的TG图片");
			try {
				std::shared_ptr<TgBot::File> img_file = tgbot.getApi().getFile(_id);
				img_content = tgbot.getApi().downloadFile(img_file->fileId);
				_file_path = _image_root_path + _id + _suffix;
				logging::debug(u8"Image", u8"完整文件路径：" + _file_path);
			}
			catch (const TgBot::TgException& e) {
				logging::error(u8"Image", u8"下载id为" + _id + "的图片失败，原因：" + std::string(e.what()));
			}
		}
		else if (_id.empty()) {
			logging::debug(u8"Image", u8"开始下载md5为" + _md5 + "的QQ图片");
			img_content = TgBot::HttpClient::getInstance().makeRequest(_url, std::vector<TgBot::HttpReqArg>());
			_file_path = _image_root_path + _md5 + _suffix;
			logging::debug(u8"Image", u8"完整文件路径：" + _file_path);
		}
		std::fstream img(_file_path, std::ios::out | std::ios::binary | std::ios::trunc);
		img << img_content;
	}

	bool Image::_get_suffix(const TgBot::Bot& tgbot) {
		try {
			std::shared_ptr<TgBot::File> img_file = tgbot.getApi().getFile(_id);
			logging::debug(u8"Image", u8"返回的file_id为" + img_file->fileId);
			_suffix = img_file->filePath.substr(img_file->filePath.find_last_of("."));
		}
		catch (const TgBot::TgException& e) {
			logging::error(u8"Image", u8"下载id为" + _id + "的图片失败，原因：" + std::string(e.what()));
			return false;
		}
		return true;
	}
}