#pragma once
#include "http_server.h"
#include "model.h"
#include "responce_handler.h"
namespace http_handler {

    namespace beast = boost::beast;
    namespace http = beast::http;
    using StringResponse = http::response<http::string_body>;

    class RequestHandler {
    public:
        explicit RequestHandler(model::Game& game)
            : game_{ game } {
        }

        RequestHandler(const RequestHandler&) = delete;
        RequestHandler& operator=(const RequestHandler&) = delete;

        template <typename Body, typename Allocator, typename Send>
        void operator()(http::request<Body, http::basic_fields<Allocator>>&& req, Send&& send) {// 
            // Обработать запрос request и отправить ответ, используя send
            if (responceHandler::BadRequestCheck(req, game_)) {
                send(responceHandler::BadRequest(req, game_));
            }
            else if (responceHandler::GetMapListCheck(req, game_)) {
                if (req.method() == http::verb::get) send(responceHandler::GetMapList(req, game_));
                else send(responceHandler::BadRequest(req, game_));
            }
            else if (responceHandler::MapNotFoundCheck(req, game_)) {
                if (req.method() == http::verb::get) send(responceHandler::MapNotFound(req, game_));
                else send(responceHandler::BadRequest(req, game_));
            }
            else if (responceHandler::GetMapByIdCheck(req, game_)) {
                if (req.method() == http::verb::get) send(responceHandler::GetMapById(req, game_));
                else send(responceHandler::BadRequest(req, game_));
            }
            else {
                send(responceHandler::PageNotFound(req, game_));
            }
        }

    private:
        model::Game& game_;

    };

}  // namespace http_handler