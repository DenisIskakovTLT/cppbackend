#include "urldecode.h"

#include <charconv>
#include <algorithm>
#include <stdexcept>
#include <regex>
#include <exception>

static int HexadecimalToDecimal(std::string hex) {
	int hexLength = hex.length();
	double dec = 0;

	for (int i = 0; i < hexLength; ++i)
	{
		char b = hex[i];

		if (b >= 48 && b <= 57)
			b -= 48;
		else if (b >= 65 && b <= 70)
			b -= 55;

		dec += b * pow(16, ((hexLength - i) - 1));
	}

	return (int)dec;
}

std::string UrlDecode(std::string_view str) {
    // Реализуйте функцию UrlDecode самостоятельно
	std::string result;
	const std::regex HexadecimalRegex(R"([0-9a-fA-F]{2})");
	int dataLen = str.length();
	std::string data{str};
	std::replace(data.begin(), data.end(), '+', ' ');
	for (int i = 0; i < dataLen; ++i)
	{
		if (data[i] == '%')
		{
			std::smatch match{};
			const std::string substr = data.substr(i + 1, 2);
			if (!std::regex_search(substr.begin(), substr.end(), match, HexadecimalRegex)) {
				throw std::invalid_argument("");
			}
			result += (char)HexadecimalToDecimal(data.substr(i + 1, 2));
			i += 2;
		}
		else
		{
			result += data[i];
		}
	}

	return result;
}
