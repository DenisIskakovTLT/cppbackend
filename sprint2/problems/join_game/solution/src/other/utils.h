#pragma once
#include <string_view>
#include <vector>

namespace requestHandler {

	std::vector<std::string_view> SplitStr(std::string_view str);
	std::string GetBearerToken(std::string_view bearer_string);

}