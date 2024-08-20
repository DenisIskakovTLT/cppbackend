#include "responce_handler.h"
#include <boost/algorithm/string.hpp>

namespace responceHandler {


    /*С бустовским split почему-то валятся тесты, 
    хотя при дебаге всё работает как надо. Оставил пока как есть.*/
    std::vector<std::string_view> SplitStr(std::string_view str) {
        std::vector<std::string_view> result;
        std::string delim = "/";
        if (str.empty() || str == delim) return result;
        auto tmpStr = str.substr(1);
        auto start = 0U;
        auto end = tmpStr.find(delim);
        while (end != std::string::npos)
        {
            result.push_back(tmpStr.substr(start, end - start));
            start = end + delim.length();
            end = tmpStr.find(delim, start);
        }
        result.push_back(tmpStr.substr(start, end));
        return result;
    };
}