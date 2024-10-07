#pragma once
#include "application.h"
#include "tokens.h"
#include "json_handler.h"
#include "utils.h"

#include <vector>
#include <boost/beast/http.hpp>
#include <boost/url/params_view.hpp>
#include <boost/url/parse.hpp>

namespace requestHandler {

    namespace beast = boost::beast;
    namespace http = beast::http;
    using StringResponse = http::response<http::string_body>;

    const size_t TWO_SEGMENT_SIZE = 2;
    const size_t THREE_SEGMENT_SIZE = 3;
    const size_t FOUR_SEGMENT_SIZE = 4;
    const size_t FIVE_SEGMENT_SIZE = 5;

    template <typename Request>
    bool BadRequestCheck(const Request& req) {
        auto url = SplitStr(req.target());
        return !url.empty() &&
            url[0] == "api" &&
            (
                url.size() > FIVE_SEGMENT_SIZE ||
                url.size() < THREE_SEGMENT_SIZE ||
                (url.size() >= TWO_SEGMENT_SIZE &&
                    url[1] != "v1") ||
                (url.size() >= THREE_SEGMENT_SIZE &&
                    url[2] != "maps" &&
                    url[2] != "game" &&
                    url[3] != "join" &&
                    url[3] != "players" &&
                    url[3] != "state" &&
                    url[3] != "player" &&
                    (url.size() == FIVE_SEGMENT_SIZE && url[4] != "action"))
                );
    };

    template <typename Request, typename Send>
    std::optional<size_t> BadRequest(
        const Request& req,
        std::shared_ptr<app::Application> application,
        Send&& send) {
        StringResponse response(http::status::bad_request, req.version());
        response.set(http::field::content_type, "application/json");
        response.body() = jsonOperation::BadRequest();
        response.content_length(response.body().size());
        response.keep_alive(req.keep_alive());
        send(response);
        return std::nullopt;
    };

    template <typename Request>
    bool GetMapListCheck(const Request& req) {
        return req.target() == "/api/v1/maps" || req.target() == "/api/v1/maps/";
    }

    template <typename Request, typename Send>
    std::optional<size_t> GetMapList(
        const Request& req,
        std::shared_ptr<app::Application> application,
        Send&& send) {
        StringResponse response(http::status::ok, req.version());
        response.set(http::field::content_type, "application/json");
        response.set(http::field::cache_control, "no-cache");
        response.body() = jsonOperation::GameToJson(application->ListMap());
        response.content_length(response.body().size());
        response.keep_alive(req.keep_alive());
        send(response);
        return std::nullopt;
    }

    template <typename Request>
    bool GetMapByIdCheck(const Request& req) {
        auto url = SplitStr(req.target());
        return url.size() == FOUR_SEGMENT_SIZE &&
            url[0] == "api" &&
            url[1] == "v1" &&
            url[2] == "maps";
    }

    template <typename Request, typename Send>
    std::optional<size_t> GetMapById(
        const Request& req,
        std::shared_ptr<app::Application>application,
        Send&& send) {
        auto id = SplitStr(req.target())[3];
        auto map = application->FindMap(model::Map::Id(std::string(id)));
        if (map == nullptr) {
            return 0;
        }
        http::response<http::string_body> response(http::status::ok, req.version());
        response.set(http::field::content_type, "application/json");
        response.set(http::field::cache_control, "no-cache");
        response.body() = jsonOperation::MapToJson(*map);
        response.content_length(response.body().size());
        response.keep_alive(req.keep_alive());
        send(response);
        return std::nullopt;
    };

    template <typename Request, typename Send>
    std::optional<size_t> MapNotFound(
        const Request& req,
        std::shared_ptr<app::Application> application,
        Send&& send) {
        StringResponse response(http::status::not_found, req.version());
        response.set(http::field::content_type, "application/json");
        response.set(http::field::cache_control, "no-cache");
        response.body() = jsonOperation::MapNotFound();
        response.content_length(response.body().size());
        response.keep_alive(req.keep_alive());
        send(response);
        return std::nullopt;
    };

    template <typename Request>
    bool JoinToGameInvalidJsonCheck(const Request& req) {
        return (req.target() == "/api/v1/game/join" || req.target() == "/api/v1/game/join/") &&
            !jsonOperation::ParseJoinToGame(req.body());
    }

    template <typename Request, typename Send>
    std::optional<size_t> JoinToGameInvalidJson(
        const Request& req,
        std::shared_ptr<app::Application> application,
        Send&& send) {
        StringResponse response(http::status::bad_request, req.version());
        response.set(http::field::content_type, "application/json");
        response.set(http::field::cache_control, "no-cache");
        response.body() = jsonOperation::JoinToGameInvalidArgument();
        response.content_length(response.body().size());
        response.keep_alive(req.keep_alive());
        send(response);
        return std::nullopt;
    }

    template <typename Request>
    bool JoinToGameEmptyPlayerNameCheck(const Request& req) {
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
    std::optional<size_t> JoinToGameEmptyPlayerName(
        const Request& req,
        std::shared_ptr<app::Application> application,
        Send&& send) {
        StringResponse response(http::status::bad_request, req.version());
        response.set(http::field::content_type, "application/json");
        response.set(http::field::cache_control, "no-cache");
        response.body() = jsonOperation::JoinToGameEmptyPlayerName();
        response.content_length(response.body().size());
        response.keep_alive(req.keep_alive());
        send(response);
        return std::nullopt;
    }

    template <typename Request>
    bool JoinToGameCheck(const Request& req) {
        return (req.target() == "/api/v1/game/join" || req.target() == "/api/v1/game/join/");
    }

    template <typename Request, typename Send>
    std::optional<size_t>  JoinToGame(
        const Request& req,
        std::shared_ptr<app::Application> application,
        Send&& send) {
        auto [player_name, map_id] = jsonOperation::ParseJoinToGame(req.body()).value();
        if (application->FindMap(map_id) == nullptr) {
            return 0;
        }
        StringResponse response(http::status::ok, req.version());
        auto session = application->GetGameSessionByMapId(map_id);
        std::string respBody;
        if (session) {
            boost::asio::dispatch(*(session.value()->GetStrand()), [application, &player_name, &map_id, &respBody] {
                auto [token, player_id] = application->JoinGame(player_name, map_id);
                respBody = jsonOperation::JoinToGame(*token, *player_id);
                });
        }
        else {
            auto [token, player_id] = application->JoinGame(player_name, map_id);
            respBody = jsonOperation::JoinToGame(*token, *player_id);
        }

        response.set(http::field::content_type, "application/json");
        response.set(http::field::cache_control, "no-cache");
        response.body() = respBody;
        response.content_length(response.body().size());
        response.keep_alive(req.keep_alive());
        send(response);
        return std::nullopt;
    }

    template <typename Request, typename Send>
    std::optional<size_t> JoinToGameMapNotFound(
        const Request& req,
        std::shared_ptr<app::Application> application,
        Send&& send) {
        StringResponse response(http::status::not_found, req.version());
        response.set(http::field::content_type, "application/json");
        response.set(http::field::cache_control, "no-cache");
        response.body() = jsonOperation::JoinToGameMapNotFound();
        response.content_length(response.body().size());
        response.keep_alive(req.keep_alive());
        send(response);
        return std::nullopt;
    }

    template <typename Request, typename Send>
    std::optional<size_t> OnlyPostMethodAllowed(
        const Request& req,
        std::shared_ptr<app::Application> application,
        Send&& send) {
        StringResponse response(http::status::method_not_allowed, req.version());
        response.set(http::field::content_type, "application/json");
        response.set(http::field::cache_control, "no-cache");
        response.set(http::field::allow, "POST");
        response.body() = jsonOperation::OnlyPostMethodAllowed();
        response.content_length(response.body().size());
        response.keep_alive(req.keep_alive());
        send(response);
        return std::nullopt;
    }

    template <typename Request>
    bool EmptyAuthorizationCheck(const Request& req) {
        return ((req.target() == "/api/v1/game/players" ||
            req.target() == "/api/v1/game/players/") ||
            (req.target() == "/api/v1/game/state" ||
                req.target() == "/api/v1/game/state/")) &&
            (req[http::field::authorization].empty() ||
                GetBearerToken(req[http::field::authorization]).empty());
    };

    template <typename Request, typename Send>
    std::optional<size_t> EmptyAuthorization(
        const Request& req,
        std::shared_ptr<app::Application> application,
        Send&& send) {
        StringResponse response(http::status::unauthorized, req.version());
        response.set(http::field::content_type, "application/json");
        response.set(http::field::cache_control, "no-cache");
        response.body() = jsonOperation::EmptyAuthorization();
        response.content_length(response.body().size());
        response.keep_alive(req.keep_alive());
        send(response);
        return std::nullopt;
    };

    template <typename Request>
    bool GetPlayersListCheck(const Request& req) {
        return (req.target() == "/api/v1/game/players" || req.target() == "/api/v1/game/players/");
    };

    template <typename Request, typename Send>
    std::optional<size_t> GetPlayersList(
        const Request& req,
        std::shared_ptr<app::Application> application,
        Send&& send) {
        auth::Token token{ GetBearerToken(req[http::field::authorization]) };
        if (!application->CheckPlayerByToken(token)) {
            return 0;
        }
        auto players = application->GetPlayersFromSession(token);
        std::string respBody;
        boost::asio::dispatch(*(application->GetGameSessionByToken(token).value()->GetStrand()),
            [&token, application, &respBody, &players] {
                respBody = jsonOperation::PlayersListOnMap(players);
            }
        );
        StringResponse response(http::status::ok, req.version());
        response.set(http::field::content_type, "application/json");
        response.set(http::field::cache_control, "no-cache");
        response.body() = respBody;
        response.content_length(response.body().size());
        response.keep_alive(req.keep_alive());
        send(response);
        return std::nullopt;
    };

    template <typename Request, typename Send>
    std::optional<size_t> InvalidMethod(
        const Request& req,
        std::shared_ptr<app::Application> application,
        Send&& send) {
        StringResponse response(http::status::method_not_allowed, req.version());
        response.set(http::field::content_type, "application/json");
        response.set(http::field::cache_control, "no-cache");
        response.set(http::field::allow, "GET, HEAD");
        response.body() = jsonOperation::InvalidMethod();
        response.content_length(response.body().size());
        response.keep_alive(req.keep_alive());
        send(response);
        return std::nullopt;
    };

    template <typename Request>
    bool GameStateCheck(const Request& req) {
        return (req.target() == "/api/v1/game/state" || req.target() == "/api/v1/game/state/");
    }

    template <typename Request, typename Send>
    std::optional<size_t> GetGameState(
        const Request& req,
        std::shared_ptr<app::Application> application,
        Send&& send) {
        auth::Token token{ GetBearerToken(req[http::field::authorization]) };
        if (!application->CheckPlayerByToken(token)) {
            return 0;
        }
        auto players = application->GetPlayersFromSession(token);
        StringResponse response(http::status::ok, req.version());
        response.set(http::field::content_type, "application/json");
        response.set(http::field::cache_control, "no-cache");
        response.body() = jsonOperation::GameState(players, application->GetGameSessionByToken(token).value()->GetLostObj());
        response.content_length(response.body().size());
        response.keep_alive(req.keep_alive());
        send(response);
        return std::nullopt;
    }

    template <typename Request>
    bool InvalidContentTypeCheck(const Request& req) {
        return ((req.target() == "/api/v1/game/join" ||
            req.target() == "/api/v1/game/join/") ||
            (req.target() == "/api/v1/game/player/action" ||
                req.target() == "/api/v1/game/player/action/")) &&
            (req[http::field::content_type].empty() ||
                req[http::field::content_type] != "application/json");
    }

    template <typename Request, typename Send>
    std::optional<size_t> InvalidContentType(
        const Request& req,
        std::shared_ptr<app::Application> application,
        Send&& send) {
        StringResponse response(http::status::bad_request, req.version());
        response.set(http::field::content_type, "application/json");
        response.set(http::field::cache_control, "no-cache");
        response.body() = jsonOperation::InvalidContentType();
        response.content_length(response.body().size());
        response.keep_alive(req.keep_alive());
        send(response);
        return std::nullopt;
    }

    template <typename Request>
    bool PlayerInvalidActionCheck(const Request& req) {
        if ((req.target() == "/api/v1/game/player/action" || req.target() == "/api/v1/game/player/action/")) {
            auto res = jsonOperation::ParsePlayerActionRequest(req.body());
            if (res.has_value()) {
                return !model::JSON_TO_DIRECTION.contains(res.value());
            }
        }
        return false;
    }

    template <typename Request, typename Send>
    std::optional<size_t> PlayerInvalidAction(
        const Request& req,
        std::shared_ptr<app::Application> application,
        Send&& send) {
        StringResponse response(http::status::bad_request, req.version());
        response.set(http::field::content_type, "application/json");
        response.set(http::field::cache_control, "no-cache");
        response.body() = jsonOperation::PlayerInvalidAction();
        response.content_length(response.body().size());
        response.keep_alive(req.keep_alive());
        send(response);
        return std::nullopt;
    }

    template <typename Request>
    bool PlayerActionCheck(const Request& req) {
        return (req.target() == "/api/v1/game/player/action" || req.target() == "/api/v1/game/player/action");
    }

    template <typename Request, typename Send>
    std::optional<size_t> PlayerAction(
        const Request& req,
        std::shared_ptr<app::Application> application,
        Send&& send) {
        auth::Token token{ GetBearerToken(req[http::field::authorization]) };
        if (!application->CheckPlayerByToken(token)) {
            return 0;
        }
        auto directionStr = jsonOperation::ParsePlayerActionRequest(req.body());
        if (!directionStr) {
            return 0;
        }
        application->MovePlayer(token, model::JSON_TO_DIRECTION.at(directionStr.value()));
        StringResponse response(http::status::ok, req.version());
        response.set(http::field::content_type, "application/json");
        response.set(http::field::cache_control, "no-cache");
        response.body() = jsonOperation::PlayerAction();
        response.content_length(response.body().size());
        response.keep_alive(req.keep_alive());
        send(response);
        return std::nullopt;
    }

    template <typename Request, typename Send>
    std::optional<size_t> UnknownToken(
        const Request& req,
        std::shared_ptr<app::Application>  application,
        Send&& send) {
        StringResponse response(http::status::unauthorized, req.version());
        response.set(http::field::content_type, "application/json");
        response.set(http::field::cache_control, "no-cache");
        response.body() = jsonOperation::UnknownToken();
        response.content_length(response.body().size());
        response.keep_alive(req.keep_alive());
        send(response);
        return std::nullopt;
    }

    template <typename Request, typename Send>
    std::optional<size_t> PageNotFound(
        const Request& req,
        std::shared_ptr<app::Application> application,
        Send&& send) {
        StringResponse response(http::status::not_found, req.version());
        response.set(http::field::content_type, "application/json");
        response.body() = jsonOperation::PageNotFound();
        response.content_length(response.body().size());
        response.keep_alive(req.keep_alive());
        send(response);
        return std::nullopt;
    };

    template <typename Request>
    bool InvalidDeltaTimeCheck(const Request& req) {
        if ((req.target() == "/api/v1/game/tick" || req.target() == "/api/v1/game/tick/")) {
            auto res = jsonOperation::ParseSetDeltaTimeRequest(req.body());
            return !res.has_value();
        }
        return false;
    }

    template <typename Request, typename Send>
    std::optional<size_t> InvalidDeltaTime(
        const Request& req,
        std::shared_ptr<app::Application> application,
        Send&& send) {
        if (!application->CheckTimeManage()) {
            return 0;
        }
        StringResponse response(http::status::bad_request, req.version());
        response.set(http::field::content_type, "application/json");
        response.set(http::field::cache_control, "no-cache");
        response.body() = jsonOperation::InvalidDeltaTime();
        response.content_length(response.body().size());
        response.keep_alive(req.keep_alive());
        send(response);
        return std::nullopt;
    }


    template <typename Request>
    bool SetDeltaTimeCheck(const Request& req) {
        return (req.target() == "/api/v1/game/tick" || req.target() == "/api/v1/game/tick/");
    }

    template <typename Request, typename Send>
    std::optional<size_t> SetDeltaTime(
        const Request& req,
        std::shared_ptr<app::Application> application,
        Send&& send) {

        if (!application->CheckTimeManage()) {                                   //--tick-period передан?
            StringResponse response(http::status::bad_request, req.version());
            response.set(http::field::content_type, "application/json");
            response.set(http::field::cache_control, "no-cache");
            response.body() = jsonOperation::InvalidEndpoint();
            response.content_length(response.body().size());
            response.keep_alive(req.keep_alive());
            send(response);
        }
        else {
            auto delta_time = jsonOperation::ParseSetDeltaTimeRequest(req.body());
            if (!delta_time.has_value()) {
                return std::nullopt;
            }
            std::chrono::milliseconds dtime(delta_time.value());
            application->UpdateGameState(dtime);
            StringResponse response(http::status::ok, req.version());
            response.set(http::field::content_type, "application/json");
            response.set(http::field::cache_control, "no-cache");
            response.body() = jsonOperation::SetDeltaTime();
            response.content_length(response.body().size());
            response.keep_alive(req.keep_alive());
            send(response);
        }

        return std::nullopt;
    }

    template <typename Request, typename Send>
    std::optional<size_t> InvalidEndpoint(
        const Request& req,
        std::shared_ptr<app::Application> application,
        Send&& send) {
        StringResponse response(application->CheckTimeManage() ? http::status::method_not_allowed : http::status::bad_request, req.version());
        response.set(http::field::content_type, "application/json");
        response.set(http::field::cache_control, "no-cache");
        response.body() = application->CheckTimeManage() ? jsonOperation::InvalidMethod() : jsonOperation::InvalidEndpoint();
        response.content_length(response.body().size());
        response.keep_alive(req.keep_alive());
        send(response);
        return std::nullopt;
    }

    /*по сути приведение типа в outType*/
    template<typename outType>
    outType ReadValueFromUrl(const boost::urls::params_view& params, const std::string& key) {
        std::stringstream stream;
        outType tmpRes;
        stream << (*params.find(key)).value;
        stream >> tmpRes;
        return tmpRes;
    };

    template <typename Request>
    bool GetRecordsCheck(const Request& req) {
        return (boost::urls::url_view{ req.target() }.path() == "/api/v1/game/records/" || boost::urls::url_view{ req.target() }.path() == "/api/v1/game/records");
    }

    template <typename Request, typename Send>
    std::optional<size_t> GetRecords(
        const Request& req,
        std::shared_ptr<app::Application> application,
        Send&& send) {
        std::optional<size_t> offset;
        std::optional<size_t> limit;
        auto params = boost::urls::url_view{ req.target() }.params();

        if (params.contains("start")) {
            offset = ReadValueFromUrl<size_t>(params, "start");
        }

        if (params.contains("maxItems")) {
            limit = ReadValueFromUrl<size_t>(params, "maxItems");
        }

        auto records = application->GetRecords(offset, limit);
        if (!records) {
            return std::nullopt;
        }

        StringResponse response(http::status::ok, req.version());
        response.set(http::field::content_type, "application/json");
        response.set(http::field::cache_control, "no-cache");
        response.body() = jsonOperation::RecordTable(records.value());
        response.content_length(response.body().size());
        response.keep_alive(req.keep_alive());
        send(response);
        return std::nullopt;
    }



}