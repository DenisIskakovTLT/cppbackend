#pragma once
#include "../file_system/file_system.h" 
#include "../logger/logger.h"

#include <vector>
#include <boost/beast/http.hpp>
#include <unordered_map>
#include <string>
#include <iostream>



namespace responceHandler {

    namespace beast = boost::beast;
    namespace http = beast::http;
    namespace sys = boost::system;
    using StringResponse = http::response<http::string_body>;
    using namespace std::literals;

    const std::unordered_map<std::string, std::string> CONTENT_TYPE = {                 //Тут типы файлов, которые будем использовать
        {".htm", "text/html"},
        {".html", "text/html"},
        {".css", "text/css"},
        {".txt", "text/plain"},
        {".js", "text/javascript"},
        {".json", "application/json"},
        {".xml", "application/xml"},
        {".png", "image/png"},
        {".jpg", "image/jpeg"},
        {".jpe", "image/jpeg"},
        {".jpeg", "image/jpeg"},
        {".gif", "image/gif"},
        {".bmp", "image/bmp"},
        {".ico", "image/vnd.microsoft.icon"},
        {".tiff", "image/tiff"},
        {".tif", "image/tiff"},
        {".svg", "image/svg+xml"},
        {".svgz", "image/svg+xml"},
        {".mp3", "audio/mpeg"}
    };

    const std::string INDEX_FILE_NAME{ "index.html" };


    template <typename Body, typename Allocator>
    bool StaticContentFileNotFoundCheck(const http::request<Body, http::basic_fields<Allocator>>& req, const std::filesystem::path& staticContentPath) {
        std::filesystem::path staticContent{ staticContentPath };
        if (req.target().empty() || req.target() == "/") {
            std::filesystem::path indexPath{ INDEX_FILE_NAME };
            staticContent = std::filesystem::weakly_canonical(staticContent / indexPath);
        }
        else {
            std::string_view pathStr = req.target().substr(1, req.target().size() - 1);
            std::filesystem::path indexPath{ pathStr };
            staticContent = std::filesystem::weakly_canonical(staticContent / indexPath);
            if (std::filesystem::is_directory(staticContent)) {
                std::filesystem::path indexPath{ INDEX_FILE_NAME };
                staticContent = std::filesystem::weakly_canonical(staticContent / indexPath);
            }
        }
        return !std::filesystem::exists(staticContent);
    };

    template <typename Body, typename Allocator>
    StringResponse StaticContentFileNotFound(const http::request<Body, http::basic_fields<Allocator>>& req, const std::filesystem::path& staticContentPath) {
        StringResponse response(http::status::not_found, req.version());
        response.set(http::field::content_type, "text/plain");
        response.body() = "Static content file not found.";
        response.content_length(response.body().size());
        response.keep_alive(req.keep_alive());
        return response;
    };


    template <typename Body, typename Allocator>
    bool LeaveStaticContentRootDirCheck(const http::request<Body, http::basic_fields<Allocator>>& req, const std::filesystem::path& staticContentPath) {
        std::filesystem::path staticContent{ staticContentPath };
        std::string_view pathStr = req.target().substr(1, req.target().size() - 1);
        std::filesystem::path tmpPath{ pathStr };
        staticContent = std::filesystem::weakly_canonical(staticContent / tmpPath);
        return !userFileSystem::IsSubPath(staticContent, staticContentPath);
    };

    template <typename Body, typename Allocator>
    StringResponse LeaveStaticContentRootDir(const http::request<Body, http::basic_fields<Allocator>>& req, const std::filesystem::path& staticContentPath) {
        StringResponse response(http::status::bad_request, req.version());
        response.set(http::field::content_type, "text/plain");
        response.body() = "Leave static content root directory.";
        response.content_length(response.body().size());
        response.keep_alive(req.keep_alive());
        return response;
    };


    template <typename Body, typename Allocator>
    bool GetStaticContentFileCheck(const http::request<Body, http::basic_fields<Allocator>>& req, const std::filesystem::path& staticContentPath) {
        return true;            //тут пока всегда true
    };

    template <typename Body, typename Allocator>
    http::response<http::file_body> GetStaticContentFile( const http::request<Body, http::basic_fields<Allocator>>& req, const std::filesystem::path& staticContentPath) {
        http::response<http::file_body> tmpRes;
        tmpRes.version(11);                            // это версия хттп 1.1
        tmpRes.result(http::status::ok);

        std::filesystem::path staticContent{ staticContentPath };
        if (req.target().empty() || req.target() == "/") {
            std::filesystem::path indexPath{ INDEX_FILE_NAME };
            staticContent = std::filesystem::weakly_canonical(staticContent / indexPath);
        }
        else {
            std::string_view pathStr = req.target().substr(1, req.target().size() - 1);
            std::filesystem::path indexPath{ pathStr };
            staticContent = std::filesystem::weakly_canonical(staticContent / indexPath);
        }
        if (CONTENT_TYPE.contains(staticContent.extension().string())) {
            tmpRes.insert(http::field::content_type, CONTENT_TYPE.at(staticContent.extension().string()));
        }
        else {
            tmpRes.insert(http::field::content_type, "application/octet-stream");
        }

        http::file_body::value_type file;

        /*Преобразование в конст чар, напрямую путь не хочет преобразовывать*/
        std::string staticContentStr = staticContent.string();
        const char* staticContentPtr = staticContentStr.c_str();

        if (sys::error_code ec; file.open(staticContentPtr, beast::file_mode::read, ec), ec) {
            BOOST_LOG_TRIVIAL(error) << logger::CreateLogMessage("error"sv,
                logger::ExceptionLog(0,
                    "Failed to open static content file "sv,
                    ec.what()));
        }
        else {
            tmpRes.body() = std::move(file);
        }
        tmpRes.prepare_payload();              //Заполнит заголовки Content-Length и Transfer-Encoding
        return tmpRes;
    };

}