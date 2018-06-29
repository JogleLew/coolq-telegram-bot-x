#include "./Image.h"

namespace logging = ctbx::logging;

namespace ctbx::image {

	/*
		受不了了
		迟早要重构
		2018/6/23

		啊，好菜的C++
		2018/6/27
	*/

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
		if (_image_root_path == "")
			_get_root();
	}

	Image::Image(const TgBot::PhotoSize::Ptr& p, const TgBot::Bot& tgbot) : Image() {
		_id = p->fileId;
		_file_size = p->fileSize;
		_height = p->height;
		_width = p->width;
		_is_valid = _get_suffix(tgbot);
		if (_image_root_path == "")
			_get_root();
	}

	Image::Image(const TgBot::Sticker::Ptr& p, const TgBot::Bot& tgbot) {
		_id = p->fileId;
		_file_size = p->fileSize;
		_height = p->height;
		_width = p->width;
		_is_valid = _get_suffix(tgbot);
		if (_image_root_path == "")
			_get_root();
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

	bool Image::send_to_tg(const int64_t chat_id, const TgBot::Bot& tgbot, const std::string& caption, const types::SOFTWARE_TYPE from){
		logging::info(u8"Image", u8"开始发送md5为" + _md5 + "的图片");
		if (!_is_valid) {
			logging::warning(u8"Image", u8"无效的图片发送！md5=" + _md5 + " _id:" + _id);
			return false;
		}
		auto send_impl = [&](const std::string& id) {
			try {
				tgbot.getApi().sendPhoto(chat_id, id, caption);
			}
			catch (const TgBot::TgException& e) {
				tgbot.getApi().sendDocument(chat_id, id, caption);
			}
		};
		if (_cq_cache.count(_md5)) {
			try {
				send_impl(_cq_cache.at(_md5));
				return true;
			}
			catch (const TgBot::TgException& e) {
				logging::error(u8"Image", u8"发送md5为" + _md5 + "的已缓存图片失败，原因：" + std::string(e.what()));
				_cq_cache.erase(_cq_cache.find(_md5));
				return false;
			}
		}
		else if (from == types::SOFTWARE_TYPE::TG && !_id.empty()) {
			try {
				send_impl(_id);
				return true;
			}
			catch (const TgBot::TgException& e) {
				logging::error(u8"Image", u8"发送id为" + _id + "的图片失败，原因：" + std::string(e.what()));
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
				logging::error(u8"Image", u8"发送md5为" + _md5 + "的图片失败，原因：" + std::string(e.what()));
				return false;
			}
			catch (const std::out_of_range& e) {
				logging::error(u8"Image", u8"返回的图片id获取失败！原因:" + std::string(e.what()));
				return false;
			}
		}
	}

	bool Image::send_to_qq(const int64_t group_id, const TgBot::Bot& tgbot, const std::string& caption, const types::SOFTWARE_TYPE from){
		// TODO : 对于QQ可以考虑在一条消息内发送多个图片
		logging::info(u8"Image", u8"开始发送id为" + _id + "的图片");
		if (!_is_valid) {
			logging::warning(u8"Image", u8"无效的图片发送！md5=" + _md5 + " _id:" + _id);
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
			logging::error(u8"Image", u8"发送id为" + _id + "路径为" + _file_path + "的图片失败，原因" + std::string(e.what()));
			return false;
		}
		return true;
	}

	bool Image::send(const ctbx::types::Group& group, const TgBot::Bot& tgbot, const std::string& caption="", const types::SOFTWARE_TYPE from = types::SOFTWARE_TYPE::TG){
		if (group.type == ctbx::types::SOFTWARE_TYPE::QQ)
			return send_to_qq(group.group_id, tgbot, caption, from);
		else
			return send_to_tg(group.group_id, tgbot, caption, from);
	}

	std::string Image::get_md5() { return _md5; }
	std::string Image::get_id() { return _id; }

	void Image::_get_root() {
		logging::debug(u8"Image", u8"开始获取图片目录");
		_image_root_path = cq::utils::ansi(cq::dir::root());
		_image_root_path += "data\\image\\";
		logging::debug(u8"Image", u8"图片目录为:" + _image_root_path);
	}

	bool Image::_download(const TgBot::Bot& tgbot) {
		if (!_is_valid)
			return false;
		if (!_md5.empty() && !_id.empty())
			return false;
		std::string img_content;
		if (_md5.empty()) {
			logging::debug(u8"Image", u8"开始下载id为" + _id + "的TG图片");
			try {
				std::shared_ptr<TgBot::File> img_file = tgbot.getApi().getFile(_id);
				img_content = tgbot.getApi().downloadFile(img_file->filePath);
				_file_path = _image_root_path + _id + _suffix;
				logging::debug(u8"Image", u8"完整文件路径：" + _file_path);
				if (_suffix == ".webp") {
					_is_valid = _convert_webp(img_content);
					return _is_valid;
				}
			}
			catch (const TgBot::TgException& e) {
				logging::error(u8"Image", u8"下载id为" + _id + "的图片失败，原因：" + std::string(e.what()));
				_is_valid = false;
				return false;
			}
			catch (const std::exception& e) {
				logging::error(u8"Image", u8"下载id为" + _id + "的图片失败，原因：" + std::string(e.what()));
				_is_valid = false;
				return false;
			}
		}
		else if (_id.empty()) {
			logging::debug(u8"Image", u8"开始下载md5为" + _md5 + "的QQ图片");
			try {
				img_content = TgBot::HttpClient::getInstance().makeRequest(_url, std::vector<TgBot::HttpReqArg>());
				_file_path = _image_root_path + _md5 + _suffix;
				logging::debug(u8"Image", u8"完整文件路径：" + _file_path);
			}
			catch (const std::exception& e) {
				logging::error(u8"Image", u8"下载md5为" + _md5 + "的图片失败，原因：" + std::string(e.what()));
				_is_valid = false;
				return false;
			}
		}
		std::fstream img(_file_path, std::ios::out | std::ios::binary | std::ios::trunc);
		img << img_content;
		return true;
	}

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
			logging::error(u8"Image", "webp配置初始化错误。");
			return false;
		}
		if (!LoadWebP(_file_path.c_str(), &data, &data_size, bitstream)) {
			logging::error(u8"Image", "读取webp错误，路径：" + _file_path);
			return false;
		}
		output_buffer->colorspace = bitstream->has_alpha ? MODE_RGBA : MODE_RGB;
		if (DecodeWebP(data, data_size, &config) != VP8_STATUS_OK) {
			logging::error(u8"Image", "解码webp错误");
			return false;
		}
		_suffix = ".png";
		_file_path = _image_root_path + _id + _suffix;
		WebPSaveImage(output_buffer, WebPOutputFileFormat::PNG, _file_path.c_str());
		free_res();
		return true;
	}

	bool Image::_get_suffix(const TgBot::Bot& tgbot) {
		try {
			std::shared_ptr<TgBot::File> img_file = tgbot.getApi().getFile(_id);
			logging::debug(u8"Image", u8"返回的file_id为" + img_file->fileId);
			logging::debug(u8"Image", u8"返回的file_path为" + img_file->filePath);
			_suffix = img_file->filePath.substr(img_file->filePath.find_last_of("."));
		}
		catch (const TgBot::TgException& e) {
			logging::error(u8"Image", u8"下载id为" + _id + "的图片失败，原因：" + std::string(e.what()));
			return false;
		}
		catch (const std::exception& e) {
			logging::error(u8"Image", u8"下载id为" + _id + "的图片失败，原因：" + std::string(e.what()));
			return false;
		}
		return true;
	}
}