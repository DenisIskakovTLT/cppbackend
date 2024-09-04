#include "htmldecode.h"

#include <unordered_map>

std::string HtmlDecode(std::string_view str) {
    html_entities_decoder::html_entities_decoder head;
    std::string tmpStr(str);
    std::stringstream outStream;
    outStream << head.decode_html_entities(tmpStr);
    return outStream.str();
}
