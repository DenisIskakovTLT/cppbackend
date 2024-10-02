#pragma once
#include <boost/beast/core.hpp>
#include <string_view>

namespace errorHandler {

	namespace beast = boost::beast;

	void ErrorLog(beast::error_code ec, std::string_view what);

}