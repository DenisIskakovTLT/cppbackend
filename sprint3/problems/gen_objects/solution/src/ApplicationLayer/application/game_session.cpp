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
        lostObjects_[tmpLostObj->GetId()] = tmpLostObj;
    };

    const GameSession::lostObjectsId& GameSession::GetLostObj() {
        return lostObjects_;
    }
}