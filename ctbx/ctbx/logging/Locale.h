#pragma once
#include<string>

#define LANG_ZH_CN

#ifdef RUNTIME_DYNAMIC
#define IS_DYNAMIC
#else
#define IS_DYNAMIC const
#endif

namespace ctbx::logging {
#ifdef LANG_ZH_CN
	// The reason why I refused to use pure macro.
	// 1. Namespace
	// 2. Extensible (dynamic switching)
	// 3. I believe in compiler's optimization. :)
	// Waiting for a better solution...
	namespace bot {
		IS_DYNAMIC std::string
			INFO_RECV_MSG_FROM_TG_TAG = u8"Forward";
		IS_DYNAMIC std::string
			INFO_RECV_MSG_FROM_TG = u8"从TG群{0}收到消息:\"{1}\"，开始转发到{2}群{3}";
		IS_DYNAMIC std::string
			DEBUG_TG_MSG_TAG = u8"TGMsgDebug";
		IS_DYNAMIC std::string
			DEBUG_TG_MSG_TEXT = u8"文本:\"{0}\"";
		IS_DYNAMIC std::string
			DEBUG_TG_MSG_IS_PHOTO = u8"照片数量:{0}";
		IS_DYNAMIC std::string
			DEBUG_TG_MSG_IS_FORWARD = u8"Forward:{0}";
		IS_DYNAMIC std::string
			DEBUG_TG_MSG_PHOTO = u8"照片ID:{0}";
		IS_DYNAMIC std::string
			DEBUG_TG_MSG_IS_STICKER = u8"Sticker:{0}";
		IS_DYNAMIC std::string
			DEBUG_TG_MSG_STICKER = u8"Sticker ID:{0},thumb:{1}";
		IS_DYNAMIC std::string
			DEBUG_TG_MSG_IS_AUDIO = u8"音频:{0}";
		IS_DYNAMIC std::string
			DEBUG_TG_MSG_AUDIO = u8"音频类型:{0},performer:{1},ID:{2}";

		IS_DYNAMIC std::string
			INFO_RECV_MSG_FROM_CQ_TAG = u8"Forward";
		IS_DYNAMIC std::string
			INFO_RECV_MSG_FROM_CQ = u8"从QQ群{0}收到消息:\"{1}\"，开始转发到{2}群{3}";
		IS_DYNAMIC std::string
			DEBUG_CQ_MSG_TAG = u8"MsgDebugInfo";
		IS_DYNAMIC std::string
			DEBUG_CQ_MSG_RAW = u8"CQ原始数据:\"{0}\"";
		IS_DYNAMIC std::string
			DEBUG_CQ_MSG_TYPE = u8"类型:{0}";
		IS_DYNAMIC std::string
			DEBUG_CQ_MSG_DETAIL = u8"First:{0}, Second:{1}";

		IS_DYNAMIC std::string
			INFO_BOT_RUNNING_TAG = u8"Bot";
		IS_DYNAMIC std::string
			INFO_BOT_EXITING = u8"Bot即将退出";
		IS_DYNAMIC std::string
			INFO_BOT_INITIALIZING = u8"Bot开始初始化";
		IS_DYNAMIC std::string
			DEBUG_BOT_RUNNING_TAG = INFO_BOT_RUNNING_TAG;
		IS_DYNAMIC std::string
			DEBUG_BOT_STARTING = u8"开始初始化TGBot，根据网络情况需要的时间有较大区别";
		IS_DYNAMIC std::string
			DEBUG_BOT_TOKEN = u8"TGBot的Token为:{0}";
		IS_DYNAMIC std::string
			DEBUG_BOT_FAIL_TO_START = u8"初始化失败，原因:\"{0}\"";
		IS_DYNAMIC std::string
			INFO_BOT_RETRY = u8"初始化失败，请尝试重新启动Bot";
		IS_DYNAMIC std::string
			CQ_LOG_TAG = u8"CTBX";
		IS_DYNAMIC std::string
			DEBUG_BOT_NAME = u8"TGBot初始化完成，TGBot用户名:\"{0}\"";
		IS_DYNAMIC std::string
			INFO_BOT_START_SUCCESSFULLY = u8"初始化完毕";
		IS_DYNAMIC std::string
			ERROR_BOT_RUNNING_TAG = INFO_BOT_RUNNING_TAG;
		IS_DYNAMIC std::string
			ERROR_LONG_POLL = u8"LongPoll错误，原因:\"{0}\"";
	}

	namespace UnifiedMessage {
		IS_DYNAMIC std::string
			ERROR_UNFMSG_TAG = u8"UnifiedMessage";
		IS_DYNAMIC std::string
			ERROR_CQ_SEND_MSG = u8"消息\"{0}\"发送失败，错误原因:\"{1}\"";

		IS_DYNAMIC std::string
			DEBUG_UNFMSG_TAG = ERROR_UNFMSG_TAG;
		IS_DYNAMIC std::string
			DEBUG_UNFMSG_CQ_MSG = u8"检查CQ剩余消息";
		IS_DYNAMIC std::string
			DEBUG_UNFMSG_CQ_MSG_EMPTY = u8"全部处理完毕";
		IS_DYNAMIC std::string
			DEBUG_UNFMSG_CQ_MSG_NOT_EMPTY = u8"仍有消息未处理";
		IS_DYNAMIC std::string
			DEBUG_UNFMSG_CQ_MSG_LEFT_DETAIL = u8"Type:{0}, Data:{1}";

		IS_DYNAMIC std::string
			DEBUG_UNFMSG_MSG = u8"打印UnifiedMessage信息片段";
		IS_DYNAMIC std::string
			DEBUG_UNFMSG_FORWARD = u8"Forward:{0}";
		IS_DYNAMIC std::string
			DEBUG_UNFMSG_REPLY = u8"Reply:{0}";
		IS_DYNAMIC std::string
			DEBUG_UNFMSG_PLAIN = u8"Plain:{0}";
		IS_DYNAMIC std::string
			DEBUG_UNFMSG_IMAGE = u8"Image: md5={0} id = {1}";
	}

	namespace Image {
		IS_DYNAMIC std::string
			DEBUG_IMAGE_TAG = u8"Image";
		IS_DYNAMIC std::string
			DEBUG_IMAGE_START_FETCHING_BY_MD5 = u8"开始获取md5为{0}的图片";
		IS_DYNAMIC std::string
			DEBUG_IMAGE_CACHED = u8"缓存中存在该图片";
		IS_DYNAMIC std::string
			DEBUG_IMAGE_CQ_FUCKING_BUG = u8"收到null后缀图片，强行修改为jpg";
		IS_DYNAMIC std::string
			DEBUG_IMAGE_SUFFIX = u8"后缀{0}有效，开始读取";

		IS_DYNAMIC std::string
			WARNING_CQIMG_PARSING_TAG = u8"CQIMG";
		IS_DYNAMIC std::string
			WARNING_CQIMG_MAGIC_ERROR = u8"Magic有误，无法读取";
		IS_DYNAMIC std::string
			WARNING_CQIMG_PROPERTY_ERROR = u8"属性无法读取";

		IS_DYNAMIC std::string
			INFO_IMAGE_TAG = DEBUG_IMAGE_TAG;
		IS_DYNAMIC std::string
			INFO_IMAGE_SEND_TO_TG = u8"开始发送md5为{0}的图片";
		IS_DYNAMIC std::string
			WARNING_IMAGE_SENDING_TAG = DEBUG_IMAGE_TAG;
		IS_DYNAMIC std::string
			WARNING_IMAGE_NOT_VALID = u8"无效的图片发送！md5={0} id={1}";
		IS_DYNAMIC std::string
			WARNING_IMAGE_CACHED_SENDING_FALIED = u8"发送md5为{0}的已缓存图片失败，原因:\"{1}\"";
		IS_DYNAMIC std::string
			DEBUG_IMAGE_SENDING_FAILED_ID = u8"发送id为{0}的图片失败，原因:\"{1}\"";
		IS_DYNAMIC std::string
			DEBUG_IMAGE_SENDING_FAILED_MD5 = u8"发送md5为{0}的图片失败，原因:\"{1}\"";
		IS_DYNAMIC std::string
			WARNING_IMAGE_FAIL_TO_GET_ID = u8"返回的图片id获取失败！原因:\"{0}\"";

		IS_DYNAMIC std::string
			INFO_IMAGE_SEND_TO_CQ = u8"开始发送id为{0}的图片";
		IS_DYNAMIC std::string
			DEBUG_IMAGE_SENDING_FAILED_ID_PATH = u8"发送id为{0}路径为{1}的图片失败，原因:{2}";

		IS_DYNAMIC std::string
			DEBUG_IMAGE_GET_ROOT = u8"开始获取图片目录";
		IS_DYNAMIC std::string
			DEBUG_IMAGE_ROOT = u8"图片目录为:{0}";

		IS_DYNAMIC std::string
			DEBUG_DOWNLOAD_TG_IMAGE = u8"开始下载id为{0}的TG图片";
		IS_DYNAMIC std::string
			DEBUG_DOWNLOAD_FULL_IMAGE_PATH = u8"完整文件路径:{0}";
		IS_DYNAMIC std::string
			DEBUG_DOWNLOAD_TG_IMAGE_FAILED = u8"下载id为{0}的TG图片失败，原因为:\"{1}\"";
		IS_DYNAMIC std::string
			DEBUG_DOWNLOAD_CQ_IMAGE = u8"开始下载md5为{0}的QQ图片";
		IS_DYNAMIC std::string
			DEBUG_DOWNLOAD_CQ_IMAGE_FAILED = u8"下载md5为{0}的QQ图片失败，原因为:\"{1}\"";

		IS_DYNAMIC std::string
			WARNING_CONVERT_WEBP_TAG = u8"Webp";
		IS_DYNAMIC std::string
			WARNING_CONFIG_INITIALIZE_FAILED = u8"webp配置初始化错误";
		IS_DYNAMIC std::string
			WARNING_FILE_NOT_FOUND = u8"读取webp错误，路径:\"{0}\"";
		IS_DYNAMIC std::string
			WARNING_DECODE_ERROR = u8"解码webp错误";

		IS_DYNAMIC std::string
			DEBUG_IMAGE_GET_ID = u8"返回的file_id为:{0}";
		IS_DYNAMIC std::string
			DEBUG_IMAGE_GET_PATH = u8"返回的file_path为:{0}";
	}

	namespace config {
		IS_DYNAMIC std::string
			DEBUG_CTBX_CONFIG_TAG = u8"Config";
		IS_DYNAMIC std::string
			DEBUG_CTBX_CONFIG_READ = u8"读取错误:{0}";
	}

	namespace cards {
		IS_DYNAMIC std::string
			WARNING_TIMER_STARTED_TAG = u8"Cards";
		IS_DYNAMIC std::string
			WARNING_TIMER_STARTED = u8"群名片更新Timer已经启动";
		IS_DYNAMIC std::string
			DEBUG_TIMER_TAG = WARNING_TIMER_STARTED_TAG;
		IS_DYNAMIC std::string
			DEBUG_TIMER_STOPPED = u8"群名片Timer停止";
		IS_DYNAMIC std::string
			INFO_TIMER_STARTING_TAG = WARNING_TIMER_STARTED_TAG;
		IS_DYNAMIC std::string
			INFO_TIMER_STARTING = u8"群名片Timer启动，更新间隔:{0}s";

		IS_DYNAMIC std::string
			INFO_UPDATE_TAG = WARNING_TIMER_STARTED_TAG;
		IS_DYNAMIC std::string
			INFO_UPDATE_CARDS = u8"开始更新群{0}名片";
		IS_DYNAMIC std::string
			DEBUG_UPDATE_TAG = WARNING_TIMER_STARTED_TAG;
		IS_DYNAMIC std::string
			DEBUG_UPDATE_ERROR = u8"群{0}名片更新失败，跳过，错误原因:{1}";
		IS_DYNAMIC std::string
			INFO_UPDATED = u8"群{0}名片已经更新";

		IS_DYNAMIC std::string
			DBUEG_FETCH_CARD = u8"单独获取群{0}成员{1}的群名片";
		IS_DYNAMIC std::string
			DEBUG_FETCH_CARD_ERROR = u8"获取名片失败，默认返回QQ号，错误原因:{0}";
	}

	// This part of localization should be replaced by
	// implementation in Localization.cpp in the future
	// because it aims to tell user how to configure ctbx
	// correctly not to help developers to debug.
	//
	// But the part of localization below justs serves
	// developers for further debugging, I wonder if
	// it is neccessary to be hot-switch. Maybe providing
	// an option in config.json is enough, right?
	namespace main {
		IS_DYNAMIC std::string
			DEBUG_MAIN_TAG = u8"CTBX";
		IS_DYNAMIC std::string
			INFO_MAIN_TAG = DEBUG_MAIN_TAG;
		IS_DYNAMIC std::string
			WARNING_MAIN_TAG = DEBUG_MAIN_TAG;
		IS_DYNAMIC std::string
			ERROR_MAIN_TAG = DEBUG_MAIN_TAG;
		IS_DYNAMIC std::string
			DEBUG_SPYLOG_FAILED = u8"spdlog初始化失败，原因:{0}";
		IS_DYNAMIC std::string
			WARNING_PERMISSION_PROBLEM = u8"日志初始化失败，是不是没有给写入权限?";
		IS_DYNAMIC std::string
			INFO_CTBX_STARTED = u8"Bot已启动，log文件请到APP文件夹查看";
		IS_DYNAMIC std::string
			ERROR_CONFIG_INVALID = u8"Bot配置文件无效，请修改配置文件后重启Bot";
		IS_DYNAMIC std::string
			INFO_CTBX_STOPPED = u8"Bot已停止";
	}
#endif LANG_ZH_CN
}