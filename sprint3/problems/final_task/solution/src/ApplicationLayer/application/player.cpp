#include "player.h"

namespace app {

    void Player::SetGameSession(std::shared_ptr<GameSession> session) {
        session_ = session;
    };

    void Player::SetDog(const std::string& dog_name, const model::Map& map, bool randomize_spawn_points) {
        dog_ = std::make_shared<model::Dog>(dog_name);
        if (randomize_spawn_points) {
            PutDogInRndPosition(map);
        }
        else {
            PutDogInStartPosition(map);
        }

        session_->GetDogs()[dog_->GetId()] = dog_;
    };

    const Player::Id& Player::GetId() const {
        return id_;
    };

    const std::string& Player::GetName() const {
        return name_;
    };

    const GameSession::Id& Player::GetSessionId() const {
        return session_->GetId();
    };

    std::shared_ptr<model::Dog> Player::GetDog() {
        return dog_;
    };

    std::shared_ptr<GameSession> Player::GetSession() {
        return session_;
    };

    void Player::PutDogInRndPosition(const model::Map& map) {
        double x, y;
        auto roads = map.GetRoads();
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
        dog_->SetPosition({ x, y });
    };

    void Player::PutDogInStartPosition(const model::Map& map) {
        double x, y;
        auto roads = map.GetRoads();
        auto road = roads[0];
        dog_->SetPosition({ static_cast<double>(road.GetStart().x),
                            static_cast<double>(road.GetStart().y) });
    };

}