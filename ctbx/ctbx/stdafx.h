#pragma once

#include "cqsdk/cqsdk.h"
#include "tgbot/tgbot.h"
#include "json/json.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/daily_file_sink.h"
#include "fmt/ostream.h"

#include <unordered_map>
#include <map>
#include <stdint.h>
#include <string>
#include <mutex>
#include <thread>
#include <fstream>
#include <list>