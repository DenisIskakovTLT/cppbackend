#pragma once
#include "collision_detector.h"
#include "constdef.h"
#include "lost_obj.h"
#include "tagged.h"
#include "typedef.h"
#include "geom.h"

#include <chrono>
#include <memory>
#include <string>
#include <unordered_map>

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

        const Id& GetId() const;                    //Геттер на айди
        const std::string& GetName() const;         //Геттер на имя

        const Direction GetDirection() const;       //Геттер на направление
        void SetDirection(Direction direction);     //Сеттер на направление
        
        const geom::Point2D& GetPosition() const;        //Геттер на позицию
        void SetPosition(geom::Point2D position);        //Сеттер на позицию
        
        const Speed& GetSpeed() const;              //Геттер на скорость
        void SetSpeed(Speed velocity);              //Сеттер на скорость
        
        void Move(Direction direction, double speed); //Двигать собаку
        geom::Point2D CalculateNewPosition(const std::chrono::milliseconds& diffTime);      //Новая позиция собаки

        const collision_detector::Gatherer& GetGatherer() const;                    //Геттер на собирателя

        bool CheckFullBag();                                                        //Проверка что сумка полная
        bool CheckEmptyBag();                                                       //Проверка что сумка пустая
        void PickUpLoot(std::shared_ptr<LostObject> obj);                           //Поднять предмет
        void ReturnLootInOffice();                                                  //Сдать в офис найденный предмет
        const std::vector<std::shared_ptr<LostObject>>& GetBag() const;             //Геттер на сумку
        const size_t GetScore() const;                                              //Геттер на очки
    private:
        Id id_;                                                                     //айди
        std::string name_;                                                          //имя
        Direction direction_{ Direction::NORTH };                                   //направление
        geom::Point2D position_{ 0.0, 0.0 };                                        //позиция
        Speed speed_{ 0.0, 0.0 };                                                   //скорость

        std::vector<std::shared_ptr<LostObject>> bag_;                              //сумка для сбора лута
        size_t bagSize_ = 0;                                                        //Размер сумки

        collision_detector::Gatherer gatherer_{ {0.0, 0.0}, {0.0, 0.0}, DOG_WIDTH}; //Собиратель
        size_t score_ = 0;                                                          //Очки за собирательство


        void AccumulateScore(size_t score);                                         //Аккумуляция очков в поле score_
    };

}