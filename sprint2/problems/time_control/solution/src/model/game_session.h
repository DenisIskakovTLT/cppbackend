#pragma once
#include "../model/map.h"
#include "../model/god.h"
#include "../other/tagged.h"
#include "../other/utils.h"

#include <vector>
#include <memory>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>

namespace model {

    namespace net = boost::asio;

    class GameSession {
    public:
        using SessionStrand = net::strand<net::io_context::executor_type>;
        using Id = util::Tagged<std::string, GameSession>;

        GameSession(std::shared_ptr<Map> map, net::io_context& ioc) :
            map_(map),
            strand_(std::make_shared<SessionStrand>(net::make_strand(ioc))),
            id_(*(map->GetId())) {};

        std::shared_ptr<Dog> CreateDog(const std::string& name);        //Создать собаку

        /*Геттеры на айди, мар и стренд*/
        const Id& GetId() const noexcept;
        const std::shared_ptr<Map> GetMap();
        std::shared_ptr<SessionStrand> GetStrand();


    private:
        std::shared_ptr<Map> map_;                      //мапа
        std::shared_ptr<SessionStrand> strand_;         //стренд
        Id id_;                                         //айди
        std::vector< std::shared_ptr<Dog> > dogs_;      //ыектор указателей на собак

        void PutDogInRndPosition(std::shared_ptr<Dog> dog); //Установить собаку на рандомную позицию
    };

}