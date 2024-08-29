#pragma once
#include "../logger/logger.h"
#include <string_view>

namespace errorHandler {

	namespace beast = boost::beast;

	void ErrorLog(beast::error_code ec, std::string_view what);

}