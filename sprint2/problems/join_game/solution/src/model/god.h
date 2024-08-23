#pragma once
#include "../other/tagged.h"

#include <string>

namespace model {

    class Dog {
        inline static size_t cntMaxId = 0;

    public:
        using Id = util::Tagged<size_t, Dog>;

        Dog(std::string name) :
            id_(Id{ Dog::cntMaxId++ }),
            name_(name) {};

        Dog(Id id, std::string name) :
            id_(id),
            name_(name) {};

        /*Кострукторы копирования все дефолтные*/
        Dog(const Dog& other) = default;
        Dog(Dog&& other) = default;
        Dog& operator = (const Dog& other) = default;
        Dog& operator = (Dog&& other) = default;
        virtual ~Dog() = default;

        const Id& GetId() const;                //Геттер на айди
        const std::string& GetName() const;     //Геттер на имя

    private:
        Id id_;                             //айди
        std::string name_;                  //имя
    };

}