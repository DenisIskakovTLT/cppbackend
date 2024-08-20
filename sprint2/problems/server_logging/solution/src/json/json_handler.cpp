#include "json_handler.h"
#include "../model/model.h"
#include <map>
#include <sstream>
#include <boost/json/array.hpp>
#include <boost/json.hpp>                                                                   //Возврат на буст. Он читабельнее, хоть и менее понятен

/*Всё что ниже переделано на буст с jsoncpp*/
namespace jsonOperation {

    std::string GameToJson(const model::Game& game) {
        boost::json::array mapsArr;
        for (auto map : game.GetMaps()) {
            boost::json::value item = { {model::MAP_ID, *(map.GetId())},
                                        {model::MAP_NAME, map.GetName()} };
            mapsArr.push_back(item);
        }
        return boost::json::serialize(mapsArr);
    }

    std::string MapToJson(const model::Map& map) {                                              
        return boost::json::serialize(boost::json::value_from(map));
    }

    std::string MapNotFound() {
        boost::json::value msg = { {RESPONSE_CODE, "mapNotFound"},
                                    {RESPONSE_MESSAGE, "Map not found"} };
        return boost::json::serialize(msg);
    };

    std::string BadRequest() {
        boost::json::value msg = { {RESPONSE_CODE, "badRequest"},
                                    {RESPONSE_MESSAGE, "Bad request"} };
        return boost::json::serialize(msg);
    };

    std::string PageNotFound() {
        boost::json::value msg = { {RESPONSE_CODE, "pageNotFound"},
                                    {RESPONSE_MESSAGE, "Page not found"} };
        return boost::json::serialize(msg);
    };

}