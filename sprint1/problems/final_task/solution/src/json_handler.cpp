#include "json_handler.h"
#include <map>
#include <sstream>
#include <json/json.h>                                                  //замена бустовской либы, с ней удобнее

namespace jsonOperation {

    std::string GameToJson(const model::Game& game) {
        Json::Value root;
        Json::StreamWriterBuilder builder;
        const std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());        //Поток записи

        for (const auto &item : game.GetMaps()) {                                           //Идём по всему файлу, вычленяем мапы и парсим айди
            Json::Value map;
            map["id"] = (*item.GetId()).c_str();
            map["name"] = item.GetName().c_str();
            root.append(std::move(map));
        }

        std::stringstream jsonString;
        writer->write(root, &jsonString);
        return jsonString.str();
    }

    void AddRoads(const model::Map& map, Json::Value& root) {
        Json::Value roads;

        for (const auto &item : map.GetRoads()) {                                           //Идём по мапе, вычленяем дороги и парсим координаты
            Json::Value road;
            road["x0"] = item.GetStart().x;
            road["y0"] = item.GetStart().y;
            if (item.IsVertical()) road["y1"] = item.GetEnd().y;
            else road["x1"] = item.GetEnd().x;
            roads.append(std::move(road));
        }

        root["roads"] = roads;
    }

    void AddBuildings(const model::Map& map, Json::Value& root) {
        Json::Value buildings;

        for (const auto &item : map.GetBuildings()) {                                       //Идём по мапе, вычленяем здания и парсим координаты
            Json::Value building;
            building["x"] = item.GetBounds().position.x;
            building["y"] = item.GetBounds().position.y;
            building["w"] = item.GetBounds().size.width;
            building["h"] = item.GetBounds().size.height;
            buildings.append(std::move(building));
        }

        root["buildings"] = buildings;
    }

    void AddOffices(const model::Map& map, Json::Value& root) {                             
        Json::Value offices;

        for (auto item : map.GetOffices()) {                                                    //Идём по мапе, вычленяем офисы и парсим координаты
            Json::Value office;
            office["id"] = (*item.GetId()).c_str();
            office["x"] = item.GetPosition().x;
            office["y"] = item.GetPosition().y;
            office["offsetX"] = item.GetOffset().dx;
            office["offsetY"] = item.GetOffset().dy;
            offices.append(std::move(office));
        }

        root["offices"] = offices;
    }

    std::string MapToJson(const model::Map& map) {                                              
        Json::Value root;
        Json::StreamWriterBuilder builder;
        const std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
        root["id"] = (*map.GetId()).c_str();
        root["name"] = map.GetName().c_str();
        AddRoads(map, root);
        AddBuildings(map, root);
        AddOffices(map, root);
        std::stringstream jsonString;
        writer->write(root, &jsonString);
        return jsonString.str();
    }

    std::string MapNotFound() {
        Json::Value root;
        Json::StreamWriterBuilder builder;
        const std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
        root["code"] = "mapNotFound";
        root["message"] = "Map not found";
        std::stringstream jsonString;
        writer->write(root, &jsonString);
        return jsonString.str();
    };

    std::string BadRequest() {
        Json::Value root;
        Json::StreamWriterBuilder builder;
        const std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
        root["code"] = "badRequest";
        root["message"] = "Bad request";
        std::stringstream jsonString;
        writer->write(root, &jsonString);
        return jsonString.str();
    };

    std::string PageNotFound() {
        Json::Value root;
        Json::StreamWriterBuilder builder;
        const std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
        root["code"] = "pageNotFound";
        root["message"] = "Page not found";
        std::stringstream jsonString;
        writer->write(root, &jsonString);
        return jsonString.str();
    };

}