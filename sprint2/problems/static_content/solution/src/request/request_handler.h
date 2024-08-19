#pragma once
#include "../server/http_server.h"
#include "../model/model.h"
#include "../responce/responce_handler.h"
#include "../responce/static_file_handler.h"

#include <filesystem>

namespace http_handler {

    namespace beast = boost::beast;
    namespace http = beast::http;
    using StringResponse = http::response<http::string_body>;

    class RequestHandler {
    public:
        explicit RequestHandler(model::Game& game, std::filesystem::path staticContentPath)
            : game_{ game }, staticContentPath_{ staticContentPath } {
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
            else if (responceHandler::StaticContentFileNotFoundCheck(req, staticContentPath_)) {
                if (req.method() == http::verb::get || req.method() == http::verb::head) send(responceHandler::StaticContentFileNotFound(req, staticContentPath_));
                else send(responceHandler::BadRequest(req, game_));
            }
            else if (responceHandler::LeaveStaticContentRootDirCheck(req, staticContentPath_)) {
                if (req.method() == http::verb::get || req.method() == http::verb::head) send(responceHandler::LeaveStaticContentRootDir(req, staticContentPath_));
                else send(responceHandler::BadRequest(req, game_));
            }
            else if (responceHandler::GetStaticContentFileCheck(req, staticContentPath_)) {
                if (req.method() == http::verb::get || req.method() == http::verb::head) send(responceHandler::GetStaticContentFile(req, staticContentPath_));
                else send(responceHandler::BadRequest(req, game_));
            }
            else {
                send(responceHandler::PageNotFound(req, game_));
            }
        }

    private:
        model::Game& game_;                             //Объект игры
        std::filesystem::path staticContentPath_;       //Путь где зранятся файлы, static content

    };

}  // namespace http_handler