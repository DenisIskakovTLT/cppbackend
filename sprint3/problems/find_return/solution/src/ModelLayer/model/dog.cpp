#include "dog.h"
#include "constdef.h"


namespace model {

    const Dog::Id& Dog::GetId() const {
        return id_;
    };

    const std::string& Dog::GetName() const {
        return name_;
    };

    void Dog::SetDirection(Direction direction) {
        direction_ = std::move(direction);
    };

    const Direction Dog::GetDirection() const {
        return direction_;
    };

    void Dog::SetPosition(geom::Point2D position) {
        position_ = std::move(position);
        gatherer_.start_pos = gatherer_.end_pos;
        gatherer_.end_pos = position_;
    };

    const geom::Point2D& Dog::GetPosition() const {
        return position_;
    };

    void Dog::SetSpeed(Speed velocity) {
        speed_ = velocity;
    };

    const Speed& Dog::GetSpeed() const {
        return speed_;
    };

    void Dog::Move(Direction direction, double speed) {
        switch (direction) {
        case Direction::NORTH: {
            SetDirection(direction);
            SetSpeed({ 0, -speed });
            break;
        }
        case Direction::SOUTH: {
            SetDirection(direction);
            SetSpeed({ 0, speed });
            break;
        }
        case Direction::WEST: {
            SetDirection(direction);
            SetSpeed({ -speed, 0 });
            break;
        }
        case Direction::EAST: {
            SetDirection(direction);
            SetSpeed({ speed, 0 });
            break;
        }
        case Direction::UNKNOWN: {
            SetSpeed({ 0, 0 });
            break;
        }
        }
    };

    geom::Point2D Dog::CalculateNewPosition(const std::chrono::milliseconds& diffTime) {
        geom::Point2D pos = GetPosition();
        const Speed& speed = GetSpeed();
        pos.x += speed.vx * diffTime.count() / CONVERT_MS_TO_S;          //1000 - перевод из мс в с
        pos.y += speed.vy * diffTime.count() / CONVERT_MS_TO_S;
        return pos;
    };

    const collision_detector::Gatherer& Dog::GetGatherer() const {
        return gatherer_;
    };

    bool Dog::CheckFullBag() {
        return bag_.size() >= bagSize_;
    }

    bool Dog::CheckEmptyBag() {
        return bag_.size() == 0;
    }

    void Dog::ReturnLootInOffice() {
        bag_.clear();
    }

    void Dog::PickUpLoot(std::shared_ptr<LostObject> obj) {
        if (!CheckFullBag()) {
            bag_.push_back(obj);
        }
    }

    const std::vector<std::shared_ptr<LostObject>>& Dog::GetBag() const {
        return bag_;
    }
}