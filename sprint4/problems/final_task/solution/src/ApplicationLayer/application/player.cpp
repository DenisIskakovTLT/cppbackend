#include "player.h"

namespace app {

    void Player::SetGameSession(std::shared_ptr<GameSession> session) {
        session_ = session;
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

    void Player::SetDog(std::shared_ptr<model::Dog> dog) {
        dog_ = dog;
    }

}