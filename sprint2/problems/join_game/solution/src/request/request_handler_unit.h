#pragma once
#include <unordered_map>
#include <boost/beast/http.hpp>

namespace requestHandler {

    namespace beast = boost::beast;
    namespace http = beast::http;


    template<typename Activator, typename Handler>
    class RHUnit {
    public:
        RHUnit(Activator activator,
            std::unordered_map<http::verb, Handler> handlers,
            Handler fault_handler) :
            activator_(std::move(activator)),
            handlers_(std::move(handlers)),
            faultHandler_(fault_handler) {};

        RHUnit(const RHUnit& other) = default;
        RHUnit(RHUnit&& other) = default;
        RHUnit& operator = (const RHUnit& other) = default;
        RHUnit& operator = (RHUnit&& other) = default;
        virtual ~RHUnit() = default;

        Handler& GetHandler(http::verb method) {            //Геттер на хэндлер, если метода нет, то выдаст хэндл ошибок
            if (handlers_.contains(method)) {
                return handlers_[method];
            }
            else {
                return faultHandler_;
            }
        };

        Activator& GetActivator() {
            return activator_;
        };

    private:
        Activator activator_;
        std::unordered_map<http::verb, Handler> handlers_;
        Handler faultHandler_;
    };

}