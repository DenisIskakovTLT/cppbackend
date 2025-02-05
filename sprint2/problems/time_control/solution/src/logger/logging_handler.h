#pragma once
#include <string>
#include <string_view>
#include <unordered_map>
#include <boost/beast/http.hpp>
#include <boost/json.hpp>
#include <boost/date_time.hpp>


//Тут использовались перегрузки tag_invoke 
//https://www.boost.org/doc/libs/master/libs/json/doc/html/json/conversion/custom_conversions.html
//Все tag_invoke ушли в cpp файл

namespace logger {

    namespace beast = boost::beast;
    namespace http = beast::http;
    namespace json = boost::json;
    using HttpRequest = http::request<http::string_body>;
    using namespace std::literals;

    /*Тут все константы полей для логгирования*/
    const std::string IP = "ip";
    const std::string URL = "URI";
    const std::string METHOD = "method";
    const std::string RESPONSE_TIME = "response_time";
    const std::string CODE = "code";
    const std::string CONTENT_TYPE = "content_type";
    const std::string PORT = "port";
    const std::string ADDRESS = "address";
    const std::string TEXT = "text";
    const std::string WHERE = "where";
    const std::string TIMESTAMP = "timestamp";
    const std::string DATA = "data";
    const std::string MESSAGE = "message";

    struct RequestLog {                                             //Структура для запроса
        RequestLog(std::string ip_addr, const HttpRequest& req) :
            ip(ip_addr),
            url(req.target()),
            method(req.method_string()) {};

        std::string ip;
        std::string url;
        std::string method;
    };

    /*Таг инвок для RequestLogData*/
    void tag_invoke(boost::json::value_from_tag, boost::json::value& jv, const RequestLog& request);

    template <typename Body, typename Fields>
    struct ResponseLog {                                             //Структура для ответа
        ResponseLog(std::string ip_addr, long res_time, const http::response<Body, Fields>& res) :
            ip(ip_addr),
            response_time(res_time),
            code(res.result_int()),
            content_type(res[http::field::content_type]) {};

        std::string ip;
        long response_time;
        int code;
        std::string content_type;
    };

    /*Таг инвок для респонса*/
    template <typename Body, typename Fields>
    void tag_invoke(boost::json::value_from_tag, boost::json::value& jv, const ResponseLog<Body, Fields>& response) {
        jv = { {IP, json::value_from(response.ip)},
                {RESPONSE_TIME, json::value_from(response.response_time)},
                {CODE, json::value_from(response.code)},
                {CONTENT_TYPE, json::value_from(response.content_type)} };
    };

    struct ServerAddrPortLog {                                   //Адрес порт
        ServerAddrPortLog(std::string addr, uint32_t prt) :
            address(addr), port(prt) {};

        std::string address;
        uint32_t port;
    };

    /*Таг инвок для ServerAddressLog*/
    void tag_invoke(boost::json::value_from_tag, boost::json::value& jv, const ServerAddrPortLog& server_address);

    struct ExceptionLog {                                           //Исключения
        ExceptionLog(int code, std::string_view text, std::string_view where) :
            code(code), text(text), where(where) {};

        int code;
        std::string_view text;
        std::string_view where;
    };

    /*Таг инвок для исключений*/
    void tag_invoke(boost::json::value_from_tag, boost::json::value& jv, const ExceptionLog& exception);

    struct ExitCodeLog {        //Код вылета
        int code;
    };

    /*Таг для кода вылета*/
    void tag_invoke(boost::json::value_from_tag, boost::json::value& jv, ExitCodeLog const& exit_code);

    template <class T>
    struct LogMessage {                                         //Сама шапка сообщения, время и месседж
        LogMessage(std::string_view msg, T&& custom_data) :
            message(msg), data(custom_data) {
            timestamp = boost::posix_time::to_iso_extended_string(boost::posix_time::microsec_clock::local_time());
        };

        std::string_view message;
        T data;
        std::string timestamp;
    };

    /*Таг для месседжа*/
    template <class T>
    void tag_invoke(boost::json::value_from_tag, boost::json::value& jv, const LogMessage<T>& msg) {
        jv = { {TIMESTAMP, json::value_from(msg.timestamp)},
                {DATA, json::value_from(msg.data)},
                {MESSAGE, json::value_from(msg.message)} };
    };

}