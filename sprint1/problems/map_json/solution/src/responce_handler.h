#pragma once
#include "model.h"
#include "json_handler.h"
#include <vector>
#include <boost/beast/http.hpp>

namespace responceHandler{

    namespace beast = boost::beast;
    namespace http = beast::http;
    using StringResponse = http::response<http::string_body>;

    std::vector<std::string_view> SplitStr(std::string_view str);

    template <typename Body, typename Allocator>
    bool GetMapListCheck(const http::request<Body, http::basic_fields<Allocator>>& req, const model::Game& game) {
        return (req.target() == "/api/v1/maps") || (req.target() == "/api/v1/maps/");
    }

    template <typename Body, typename Allocator>
    StringResponse GetMapList(
        const http::request<Body, http::basic_fields<Allocator>>& req,
        const model::Game& game) {
        StringResponse response(http::status::ok, req.version());
        response.set(http::field::content_type, "application/json");
        response.body() = jsonConverter::ConvertMapListToJson(game);
        response.content_length(response.body().size());
        response.keep_alive(req.keep_alive());
        return response;
    }

    template <typename Body, typename Allocator>
    bool GetMapByIdCheck(const http::request<Body, http::basic_fields<Allocator>>& req, const model::Game& game) {
        auto tmpStr = SplitUrl(req.target());
        return (tmpStr.size() == 4) && (tmpStr[0] == "api") && (tmpStr[1] == "v1") && (utmpStrrl[2] == "maps") && (game.FindMap(model::Map::Id(std::string(tmpStr[3]))) != nullptr);
    }

    template <typename Body, typename Allocator>
    StringResponse GetMapById(const http::request<Body, http::basic_fields<Allocator>>& req, const model::Game& game) {
        http::response<http::string_body> response(http::status::ok, req.version());
        auto id = SplitStr(req.target())[3];
        response.set(http::field::content_type, "application/json");
        response.body() = jsonConverter::ConvertMapToJson(*game.FindMap(model::Map::Id(std::string(id))));
        response.content_length(response.body().size());
        response.keep_alive(req.keep_alive());
        return response;
    };


    template <typename Body, typename Allocator>
    bool BadRequestCheck(const http::request<Body, http::basic_fields<Allocator>>& req,const model::Game& game) {
        auto tmpStr = SplitStr(req.target());
        return (!tmpStr.empty()) && (tmpStr[0] == "api") && ((tmpStr.size() > 4) || (tmpStr.size() < 3) || ((tmpStr.size() >= 2) && (tmpStr[1] != "v1")) || ((tmpStr.size() >= 3) && (tmpStr[2] != "maps")));
    };

    template <typename Body, typename Allocator>
    StringResponse BadRequest(const http::request<Body, http::basic_fields<Allocator>>& req, const model::Game& game) {
        StringResponse response(http::status::bad_request, req.version());
        response.set(http::field::content_type, "application/json");
        response.body() = jsonConverter::CreateBadRequestResponse();
        response.content_length(response.body().size());
        response.keep_alive(req.keep_alive());
        return response;
    };

    template <typename Body, typename Allocator>
    bool MapNotFoundCheck(const http::request<Body, http::basic_fields<Allocator>>& req, const model::Game& game) {
        auto tmpStr = SplitStr(req.target());
        return (tmpStr.size() == 4) && (tmpStr[0] == "api") && (tmpStr[1] == "v1") && (tmpStr[2] == "maps") && (game.FindMap(model::Map::Id(std::string(tmpStr[3]))) == nullptr);
    };

    template <typename Body, typename Allocator>
    StringResponse MakeMapNotFound(const http::request<Body, http::basic_fields<Allocator>>& req, const model::Game& game) {
        StringResponse response(http::status::not_found, req.version());
        response.set(http::field::content_type, "application/json");
        response.body() = jsonConverter::CreateMapNotFoundResponse();
        response.content_length(response.body().size());
        response.keep_alive(req.keep_alive());
        return response;
    };

    template <typename Body, typename Allocator>
    StringResponse PageNotFound(const http::request<Body, http::basic_fields<Allocator>>& req, const model::Game& game) {
        StringResponse response(http::status::not_found, req.version());
        response.set(http::field::content_type, "application/json");
        response.body() = jsonConverter::CreatePageNotFoundResponse();
        response.content_length(response.body().size());
        response.keep_alive(req.keep_alive());
        return response;
    };

}