#pragma once
#include "../application/application.h"
#include "../auth/tokens.h"
#include "../json/json_handler.h"
#include "../other/utils.h"

#include <vector>
#include <boost/beast/http.hpp>

namespace requestHandler {

    namespace beast = boost::beast;
    namespace http = beast::http;
    using StringResponse = http::response<http::string_body>;

    const size_t TWO_SEGMENT_SIZE = 2;
    const size_t THREE_SEGMENT_SIZE = 3;
    const size_t FOUR_SEGMENT_SIZE = 4;

    template <typename Request>
    bool GetMapListСheck(
        const Request& req,
        app::Application& application) {
        return req.target() == "/api/v1/maps" || req.target() == "/api/v1/maps/";
    }

    template <typename Request, typename Send>
    void GetMapList(
        const Request& req,
        app::Application& application,
        Send&& send) {
        StringResponse response(http::status::ok, req.version());
        response.set(http::field::content_type, "application/json");
        response.body() = jsonOperation::GameToJson(application.ListMap());
        response.content_length(response.body().size());
        response.keep_alive(req.keep_alive());
        send(response);
    }


    template <typename Request>
    bool GetMapByIdCheck(
        const Request& req,
        app::Application& application) {
        auto url = SplitStr(req.target());
        return url.size() == FOUR_SEGMENT_SIZE &&
            url[0] == "api" &&
            url[1] == "v1" &&
            url[2] == "maps" &&
            application.FindMap(model::Map::Id(std::string(url[3]))) != nullptr;
    }

    template <typename Request, typename Send>
    void GetMapById(
        const Request& req,
        app::Application& application,
        Send&& send) {
        http::response<http::string_body> response(http::status::ok, req.version());
        auto id = SplitStr(req.target())[3];
        response.set(http::field::content_type, "application/json");
        response.body() = jsonOperation::MapToJson(*application.FindMap(model::Map::Id(std::string(id))));
        response.content_length(response.body().size());
        response.keep_alive(req.keep_alive());
        send(response);
    };


    template <typename Request>
    bool BadRequestCheck(
        const Request& req,
        app::Application& application) {
        auto url = SplitStr(req.target());
        return !url.empty() &&
            url[0] == "api" &&
            (
                url.size() > FOUR_SEGMENT_SIZE ||
                url.size() < THREE_SEGMENT_SIZE ||
                (url.size() >= TWO_SEGMENT_SIZE &&
                    url[1] != "v1") ||
                (url.size() >= THREE_SEGMENT_SIZE &&
                    url[2] != "maps" &&
                    url[2] != "game" &&
                    url[3] != "join" &&
                    url[3] != "players")
                );
    };

    template <typename Request, typename Send>
    void BadRequest(
        const Request& req,
        app::Application& application,
        Send&& send) {
        StringResponse response(http::status::bad_request, req.version());
        response.set(http::field::content_type, "application/json");
        response.body() = jsonOperation::BadRequest();
        response.content_length(response.body().size());
        response.keep_alive(req.keep_alive());
        send(response);
    };


    template <typename Request>
    bool MapNotFoundCheck(
        const Request& req,
        app::Application& application) {
        auto url = SplitStr(req.target());
        return url.size() == FOUR_SEGMENT_SIZE &&
            url[0] == "api" &&
            url[1] == "v1" &&
            url[2] == "maps" &&
            application.FindMap(model::Map::Id(std::string(url[3]))) == nullptr;
    };

    template <typename Request, typename Send>
    void MapNotFound(
        const Request& req,
        app::Application& application,
        Send&& send) {
        StringResponse response(http::status::not_found, req.version());
        response.set(http::field::content_type, "application/json");
        response.body() = jsonOperation::MapNotFound();
        response.content_length(response.body().size());
        response.keep_alive(req.keep_alive());
        send(response);
    };


    template <typename Request>
    bool JoinToGameInvalidJsonCheck(
        const Request& req,
        app::Application& application) {
        return (req.target() == "/api/v1/game/join" || req.target() == "/api/v1/game/join/") &&
            !jsonOperation::ParseJoinToGame(req.body());
    }

    template <typename Request, typename Send>
    void JoinToGameInvalidJson(
        const Request& req,
        app::Application& application,
        Send&& send) {
        StringResponse response(http::status::bad_request, req.version());
        response.set(http::field::content_type, "application/json");
        response.set(http::field::cache_control, "no-cache");
        response.body() = jsonOperation::JoinToGameInvalidArgument();
        response.content_length(response.body().size());
        response.keep_alive(req.keep_alive());
        send(response);
    }


    template <typename Request>
    bool JoinToGameMapNotFoundCheck(
        const Request& req,
        app::Application& application) {
        if ((req.target() == "/api/v1/game/join" || req.target() == "/api/v1/game/join/")) {
            auto res = jsonOperation::ParseJoinToGame(req.body());
            if (!res) {
                return false;
            }
            auto [player_name, map_id] = res.value();
            return application.FindMap(map_id) == nullptr;
        }
        return false;
    }

    template <typename Request, typename Send>
    void JoinToGameMapNotFound(
        const Request& req,
        app::Application& application,
        Send&& send) {
        StringResponse response(http::status::not_found, req.version());
        response.set(http::field::content_type, "application/json");
        response.set(http::field::cache_control, "no-cache");
        response.body() = jsonOperation::JoinToGameMapNotFound();
        response.content_length(response.body().size());
        response.keep_alive(req.keep_alive());
        send(response);
    }


    template <typename Request>
    bool JoinToGameEmptyPlayerNameCheck(
        const Request& req,
        app::Application& application) {
        if ((req.target() == "/api/v1/game/join" || req.target() == "/api/v1/game/join/")) {
            auto res = jsonOperation::ParseJoinToGame(req.body());
            if (!res) {
                return false;
            }
            auto [player_name, map_id] = res.value();
            return player_name.empty();
        }
        return false;
    }

    template <typename Request, typename Send>
    void JoinToGameEmptyPlayerName(
        const Request& req,
        app::Application& application,
        Send&& send) {
        StringResponse response(http::status::bad_request, req.version());
        response.set(http::field::content_type, "application/json");
        response.set(http::field::cache_control, "no-cache");
        response.body() = jsonOperation::JoinToGameEmptyPlayerName();
        response.content_length(response.body().size());
        response.keep_alive(req.keep_alive());
        send(response);
    }


    template <typename Request>
    bool JoinToGameCheck(
        const Request& req,
        app::Application& application) {
        return (req.target() == "/api/v1/game/join" || req.target() == "/api/v1/game/join/");
    }

    template <typename Request, typename Send>
    void JoinToGame(
        const Request& req,
        app::Application& application,
        Send&& send) {
        auto [player_name, map_id] = jsonOperation::ParseJoinToGame(req.body()).value();
        auto [token, player_id] = application.JoinGame(player_name, map_id);
        StringResponse response(http::status::ok, req.version());
        response.set(http::field::content_type, "application/json");
        response.set(http::field::cache_control, "no-cache");
        response.body() = jsonOperation::JoinToGame(*token, *player_id);
        response.content_length(response.body().size());
        response.keep_alive(req.keep_alive());
        send(response);
    }

    template <typename Request, typename Send>
    void OnlyPostMethodAllowed(
        const Request& req,
        app::Application& application,
        Send&& send) {
        StringResponse response(http::status::method_not_allowed, req.version());
        response.set(http::field::content_type, "application/json");
        response.set(http::field::cache_control, "no-cache");
        response.set(http::field::allow, "POST");
        response.body() = jsonOperation::OnlyPostMethodAllowed();
        response.content_length(response.body().size());
        response.keep_alive(req.keep_alive());
        send(response);
    };


    template <typename Request>
    bool EmptyAuthorizationCheck(
        const Request& req,
        app::Application& application) {
        return (req.target() == "/api/v1/game/players" ||
            req.target() == "/api/v1/game/players/") &&
            (req[http::field::authorization].empty() ||
                GetBearerToken(req[http::field::authorization]).empty());
    }

    template <typename Request, typename Send>
    void EmptyAuthorization(
        const Request& req,
        app::Application& application,
        Send&& send) {
        StringResponse response(http::status::unauthorized, req.version());
        response.set(http::field::content_type, "application/json");
        response.set(http::field::cache_control, "no-cache");
        response.body() = jsonOperation::EmptyAuthorization();
        response.content_length(response.body().size());
        response.keep_alive(req.keep_alive());
        send(response);
    }

    template <typename Request>
    bool UnknownTokenCheck(
        const Request& req,
        app::Application& application) {
        if (req.target() == "/api/v1/game/players" || req.target() == "/api/v1/game/players/") {
            auth::Token token{ GetBearerToken(req[http::field::authorization]) };
            return !application.CheckPlayerByToken(token);
        }
        return false;
    }

    template <typename Request, typename Send>
    void UnknownToken(
        const Request& req,
        app::Application& application,
        Send&& send) {
        StringResponse response(http::status::unauthorized, req.version());
        response.set(http::field::content_type, "application/json");
        response.set(http::field::cache_control, "no-cache");
        response.body() = jsonOperation::UnknownToken();
        response.content_length(response.body().size());
        response.keep_alive(req.keep_alive());
        send(response);
    }

    template <typename Request>
    bool GetPlayersListCheck(
        const Request& req,
        app::Application& application) {
        return (req.target() == "/api/v1/game/players" || req.target() == "/api/v1/game/players/");
    }

    template <typename Request, typename Send>
    void GetPlayersList(
        const Request& req,
        app::Application& application,
        Send&& send) {
        auth::Token token{ GetBearerToken(req[http::field::authorization]) };
        auto players = application.GetPlayersFromSession(token);
        StringResponse response(http::status::ok, req.version());
        response.set(http::field::content_type, "application/json");
        response.set(http::field::cache_control, "no-cache");
        response.body() = jsonOperation::PlayersListOnMap(players);
        response.content_length(response.body().size());
        response.keep_alive(req.keep_alive());
        send(response);
    }

    template <typename Request, typename Send>
    void InvalidMethod(
        const Request& req,
        app::Application& application,
        Send&& send) {
        StringResponse response(http::status::method_not_allowed, req.version());
        response.set(http::field::content_type, "application/json");
        response.set(http::field::cache_control, "no-cache");
        response.set(http::field::allow, "GET, HEAD");
        response.body() = jsonOperation::InvalidMethod();
        response.content_length(response.body().size());
        response.keep_alive(req.keep_alive());
        send(response);
    };


    template <typename Request, typename Send>
    void PageNotFound(
        const Request& req,
        app::Application& application,
        Send&& send) {
        StringResponse response(http::status::not_found, req.version());
        response.set(http::field::content_type, "application/json");
        response.body() = jsonOperation::PageNotFound();
        response.content_length(response.body().size());
        response.keep_alive(req.keep_alive());
        send(response);
    };

}