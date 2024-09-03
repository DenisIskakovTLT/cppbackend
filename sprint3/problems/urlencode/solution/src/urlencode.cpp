#include "urlencode.h"

#include <string>
#include <algorithm>
#include <sstream>
#include <unordered_set>

const std::unordered_set<char> ENCODE_CHARS = {
	'!', '#', '$', '&', '\'', '(', ')', '*', ',', '/', ':', ';', '=', '?', '@', '[', ']'
};


std::string UrlEncode(std::string_view str) {

	std::string tmpRes(str);
	std::replace(tmpRes.begin(), tmpRes.end(), ' ', '+');
	std::stringstream out;
	for (auto ch : tmpRes) {
		
		uint16_t chInt = ch;

		if (ch != '+' && (chInt < 32 || chInt > 128 || ENCODE_CHARS.contains(ch))) {
			out << "%" << std::hex << chInt;
		}
		else {
			out << ch;
		}

	}

	return out.str();
}
