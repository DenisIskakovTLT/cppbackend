#pragma once
#include "../request/api_request_handler.h"
#include "../request/request_handler_unit.h"
#include <functional>
#include <vector>

namespace requestHandler {

    namespace net = boost::asio;

    template<typename Request, typename Send>
    class ApiRequestHandlerProxy {
        using ActivatorType = bool(*)(const Request&, app::Application&);
        using HandlerType = void(*)(const Request&, app::Application&, Send&&);
    public:

        /*Всё копирование запрещено*/
        ApiRequestHandlerProxy(const ApiRequestHandlerProxy&) = delete;
        ApiRequestHandlerProxy& operator=(const ApiRequestHandlerProxy&) = delete;
        ApiRequestHandlerProxy(ApiRequestHandlerProxy&&) = delete;
        ApiRequestHandlerProxy& operator=(ApiRequestHandlerProxy&&) = delete;


        static ApiRequestHandlerProxy& GetInstance() {                 //Ссыль на объект
            static ApiRequestHandlerProxy obj;
            return obj;
        };

        bool Execute(const Request& req, app::Application& application, Send&& send) {              //Сам исполнитель
            for (auto item : requests_) {
                if (item.GetActivator()(req, application)) {
                    net::dispatch(*application.GetStrand(), [&item, &req, &application, &send] {
                        item.GetHandler(req.method())(req, application, std::move(send));
                        });
                    return true;
                }
            }
            return false;
        };

    private:

        std::vector< RHUnit<ActivatorType, HandlerType> > requests_ = {
            RHUnit<ActivatorType, HandlerType>(BadRequestCheck,
                                                            {{http::verb::get, BadRequest}},
                                                            BadRequest),
            RHUnit<ActivatorType, HandlerType>(GetMapListСheck,
                                                            {{http::verb::get, GetMapList}},
                                                            BadRequest),
            RHUnit<ActivatorType, HandlerType>(MapNotFoundCheck,
                                                            {{http::verb::get, MapNotFound}},
                                                            BadRequest),
            RHUnit<ActivatorType, HandlerType>(GetMapByIdCheck,
                                                            {{http::verb::get, GetMapById}},
                                                            BadRequest),
            RHUnit<ActivatorType, HandlerType>(JoinToGameInvalidJsonCheck,
                                                            {{http::verb::post, JoinToGameInvalidJson}},
                                                            OnlyPostMethodAllowed),
            RHUnit<ActivatorType, HandlerType>(JoinToGameEmptyPlayerNameCheck,
                                                            {{http::verb::post, JoinToGameEmptyPlayerName}},
                                                            OnlyPostMethodAllowed),
            RHUnit<ActivatorType, HandlerType>(JoinToGameMapNotFoundCheck,
                                                            {{http::verb::post, JoinToGameMapNotFound}},
                                                            OnlyPostMethodAllowed),
            RHUnit<ActivatorType, HandlerType>(JoinToGameCheck,
                                                            {{http::verb::post, JoinToGame}},
                                                            OnlyPostMethodAllowed),
            RHUnit<ActivatorType, HandlerType>(EmptyAuthorizationCheck,
                                                            {{http::verb::get, EmptyAuthorization},
                                                            {http::verb::head, EmptyAuthorization}},
                                                            InvalidMethod),
            RHUnit<ActivatorType, HandlerType>(UnknownTokenCheck,
                                                            {{http::verb::get, UnknownToken},
                                                            {http::verb::head, UnknownToken}},
                                                            InvalidMethod),
            RHUnit<ActivatorType, HandlerType>(GetPlayersListCheck,
                                                            {{http::verb::get, GetPlayersList},
                                                            {http::verb::head, GetPlayersList}},
                                                            InvalidMethod)
        };

        ApiRequestHandlerProxy() = default;
    };

}