#include "./Image.h"

namespace logging = ctbx::logging;

namespace ctbx::image {

	std::istream& operator>>(std::istream& in, Image& img) {
		std::string buffer;
		for (int i = 0; i < 7; i++) {
			std::getline(in, buffer);
			if (i == 0 && buffer == "[image]")
				continue;
			else if (i == 0 && buffer != "[image]") {
				logging::warning(u8"Image", u8"Magic有误，无法读取");
				return in;
			}
			else {
				std::size_t pos = buffer.find_first_of("=");
				if (pos == std::string::npos) {
					logging::warning(u8"Image", u8"属性无法读取");
					return in;
				}
				std::string left = buffer.substr(0, pos);
				std::string right = buffer.substr(pos+1);
				if (left == "md5")
					img._md5 = right;
				else if (left == "width")
					img._width = std::stoi(right);
				else if (left == "height")
					img._height = std::stoi(right);
				else if (left == "size")
					img._file_size = std::stoi(right);
				else if (left == "url")
					img._url = right;
				else if (left == "addtime")
					img._add_time = std::stoi(right);
			}
		}
		img._is_valid = true;
		return in;
	}

	std::string Image::_image_root_path = "";
	std::map<std::string, std::string> Image::_tg_cache = {};
	std::map < std::string, std::string > Image::_cq_cache = {};
	const std::map <std::string, std::string> Image::_mimetype = { {".jpg", "image/jpeg"}, {".png", "image/png"}, {".gif", "image/gif"} };

	const Image Image::get_image(const TgBot::PhotoSize::Ptr& p, const TgBot::Bot& tgbot) {
		logging::debug(u8"Image", u8"开始获取img_id为" + p->fileId + u8"的图片");
		Image img(p);
		try {
			std::shared_ptr<TgBot::File> img_file = tgbot.getApi().getFile(p->fileId);
			// only for test
			logging::debug(u8"Image", u8"返回的file_id为" + img_file->fileId);
			img._suffix = img_file->filePath.substr(img_file->filePath.find_last_of("."));
		}
		catch (const TgBot::TgException& e) {
			logging::error(u8"Image", u8"下载id为" + img._id + "的图片失败，原因：" + std::string(e.what()));
		}
		return img;
	}

	const Image Image::get_image(const std::string& img_name){
		std::size_t pos = img_name.find_last_of(".");
		std::string md5 = img_name.substr(0, pos);
		std::string suffix = img_name.substr(pos);
		logging::debug(u8"Image", u8"开始获取md5为" + md5 + u8"的图片");
		if (_cq_cache.count(md5))
			return Image(_cq_cache[md5], md5);
		Image cqimg;
		if (_image_root_path == "")
			_get_root();
		std::fstream img(_image_root_path + md5 + suffix + ".cqimg", std::ios::in);
		if (img.is_open()) {
			cqimg._suffix = suffix;
			logging::debug(u8"Image", u8"后缀：" + suffix + "有效，开始读取");
			img >> cqimg;
		}
		return cqimg;
	}

	void Image::send_to_tg(const int64_t chat_id, const TgBot::Bot& tgbot, const std::string& caption=""){
		if (_is_valid)
			_download(tgbot);
		else {
			logging::warning(u8"Image", u8"无效的图片发送！md5=" + _md5 + " _id:" + _id);
			return;
		}
		logging::info(u8"Image", u8"开始发送id为" + _id + "的图片");
		try {
			tgbot.getApi().sendPhoto(chat_id, TgBot::InputFile::fromFile(_file_path, _mimetype.at(_suffix)), caption);
		}
		catch (const TgBot::TgException& e) {
			logging::error(u8"Image", u8"发送id为" + _id + "的图片失败，原因：" + std::string(e.what()));
		}
	}

	void Image::send_to_qq(const int64_t group_id, const TgBot::Bot& tgbot){
		logging::warning(u8"Image", u8"没实现");
		return;
	}

	void Image::send(const ctbx::types::Group& group, const TgBot::Bot& tgbot){
		if (group.type == ctbx::types::GROUP_TYPE::QQ)
			send_to_qq(group.group_id, tgbot);
		else
			send_to_tg(group.group_id, tgbot);
	}

	Image::Image() 
		: _id(""), _md5(""), _is_valid(false), _file_path(""), _url(""), _suffix(""), _width(-1), _height(-1), _file_size(-1), _add_time(-1){}

	Image::Image(const std::string& md5, const std::string& id) : Image() {
		_md5 = md5;
		_id = id;
		_is_valid = true;
	}

	Image::Image(const TgBot::PhotoSize::Ptr& p) : Image() {
		_id = p->fileId;
		_file_size = p->fileSize;
		_height = p->height;
		_width = p->width;
		_is_valid = true;
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
}