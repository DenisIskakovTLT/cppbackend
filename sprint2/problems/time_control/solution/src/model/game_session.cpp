#include "../model/game_session.h"

namespace model {

    const GameSession::Id& GameSession::GetId() const noexcept {
        return id_;
    }

    const std::shared_ptr<Map> GameSession::GetMap() {
        return map_;
    };

    std::shared_ptr<GameSession::SessionStrand> GameSession::GetStrand() {
        return strand_;
    };

    std::shared_ptr<Dog> GameSession::CreateDog(const std::string& dog_name) {
        auto dog = std::make_shared<Dog>(dog_name);
        dogs_.push_back(dog);
        //PutDogInRndPosition(dog);
        return dog;
    };

    void GameSession::PutDogInRndPosition(std::shared_ptr<Dog> dog) {
        double x, y;
        auto roads = map_->GetRoads();
        int road_index = randomgen::RandomInt(0, roads.size() - 1);
        auto road = roads[road_index];
        if (road.IsHorizontal()) {
            x = randomgen::RandomDouble(road.GetStart().x,
                road.GetEnd().x);
            y = road.GetStart().y;
        }
        else {
            y = randomgen::RandomDouble(road.GetStart().y,
                road.GetEnd().y);
            x = road.GetStart().x;
        }
        dog->SetPosition({ x, y });
    };

}