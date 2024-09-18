#include "game_session.h"

namespace app {

    const GameSession::Id& GameSession::GetId() const noexcept {
        return id_;
    }

    const std::shared_ptr<model::Map> GameSession::GetMap() {
        return map_;
    };

    std::shared_ptr<GameSession::SessionStrand> GameSession::GetStrand() {
        return strand_;
    };

    GameSession::DogsId& GameSession::GetDogs()  {
        return dogs_;
    }

    void GameSession::GenerateLoot(const TimeInterval& interval) {

        auto tmpObjLoot = lootGenerator_.Generate(interval, lostObjects_.size(), dogs_.size());

        for (size_t i = 0; i < tmpObjLoot; ++i) {
            GenerateLostObject();
        }
    };

    void GameSession::PutLootInRndPosition(std::shared_ptr<model::LostObject> loot) {
        loot->SetPosition(map_->GenerateRndPosition());
    };

    void GameSession::GenerateLostObject() {
        auto tmpLostObj = std::make_shared<model::LostObject>();

        tmpLostObj->SetType(randomgen::RandomLootType(0, map_->GetSizeLootTypes() - 1));
        PutLootInRndPosition(tmpLostObj);
        tmpLostObj->SetValue(map_->GetLootTypeViaIdx(tmpLostObj->GetType()).value);
        lostObjects_[tmpLostObj->GetId()] = tmpLostObj;
    };

    const GameSession::lostObjectsId& GameSession::GetLostObj() {
        return lostObjects_;
    }

    void GameSession::StartGame() {

        /*Апдейтер игры. Запускаем, если не ручное управление временем*/
        if (gameRefreshPeriod_.count() != 0) {
            gameTicker_ = std::make_shared<tickerTime::Ticker>(
                strand_,
                gameRefreshPeriod_,
                std::bind(&GameSession::UpdateGameState, this, std::placeholders::_1) //https://en.cppreference.com/w/cpp/utility/functional/bind
            );
            gameTicker_->Start();
        }
    };

    void GameSession::UpdateGameState(const TimeInterval& period) {
        bool noMove = true;
        for (auto [id, dog] : dogs_) {
            auto [new_pos, new_speed] = map_->GetMove(
                dog->GetPosition(),
                dog->CalculateNewPosition(period),
                dog->GetSpeed()
            );
            if (dog->GetPosition() != new_pos) {
                noMove = false;
            }
            dog->SetPosition(new_pos);
            dog->SetSpeed(new_speed);
        }

        //сборка лута тут
        if (!noMove) {
            FindAndReturnLoot();
        }

        //Генерация лута
        GenerateLoot(period);
        
    }

    void GameSession::FindAndReturnLoot() {
        std::vector< std::shared_ptr<collision_detector::Item> > tmpItems;          //Сюда закинем весь имеющийся лут
        std::vector< std::shared_ptr<model::Dog> > tmpDogs;                         //Тут все собаки на карте
        
        /*Закидываем лут в items вектор*/
        for (auto [id, obj] : lostObjects_) {   
            tmpItems.push_back(obj);
        }
        /*Закидываем собак в dog вектор*/
        for (auto [id, dog] : dogs_) {
            tmpDogs.push_back(dog);
        }
        /*Закидываем офисы в items вектор*/
        for (auto office : map_->GetOffices()) {
            tmpItems.push_back(std::make_shared<model::Office>(office));
        }

        model::DogGather dogGather(std::move(tmpItems), std::move(tmpDogs));        //Собака искатель

        auto foundEvents = collision_detector::FindGatherEvents(dogGather);

        if (!foundEvents.empty()) {                                                 //ивенты не пустые, значит, что-то найдено
            for (const collision_detector::GatheringEvent& event : foundEvents) {   //идём по ивентам
                PickUpLoot(dogGather, event.item_id, event.gatherer_id);            //Подобрать лут
                ReturnLootInOffice(dogGather, event.item_id, event.gatherer_id);    //вернуть в офис
            }
            
        }

    }

    void GameSession::PickUpLoot(const model::DogGather& dogGather, size_t itemID, size_t gathererID) {
        //Внутри item есть type, который optional
        //Если nullopt, значит это офис, т.к. лут имеет номер типа
        if (dogGather.GetItem(itemID).GetType()) {          
            auto dog = dogs_[dogGather.GetDogIdFromIdx(gathererID)];
            auto lostObjId = dogGather.GetLostObj(itemID)->GetId();

            if (dog->CheckFullBag() || lostObjects_.contains(lostObjId))return;   //Сумка полная или такого лута нет, уходим
            
            dog->PickUpLoot(lostObjects_.at(lostObjId));
            auto pickedUpObjId = [id = lostObjId](const auto& item) {             //Предикат для поиска поднятых объектов
                auto const& [key, value] = item;
                return key == id; 
                };
            std::erase_if(lostObjects_, pickedUpObjId);                           //Удалить поднятый объект
        }
    }

    void GameSession::ReturnLootInOffice(const model::DogGather& dogGather, size_t itemID, size_t gathererID) {
        //Внутри item есть type, который optional
        //Если nullopt, значит это офис, т.к. лут имеет номер типа
        if (!dogGather.GetItem(itemID).GetType()) {
            auto dog = dogs_[dogGather.GetDogIdFromIdx(gathererID)];
            if (dog->CheckEmptyBag()) return;   //сумка пустая, выкладывать нечего
            dog->ReturnLootInOffice();
        }
    }

}