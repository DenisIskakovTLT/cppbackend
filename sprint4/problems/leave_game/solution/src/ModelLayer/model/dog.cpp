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

    void Dog::SetSpeed(Speed speed) {
        speed_ = speed;

        if (speed == Speed{0, 0}) {
            activity_ = DogActivity::STOP;
        }
        else {
            activity_ = DogActivity::RUN;
        }
        
    };

    const Speed& Dog::GetSpeed() const {
        return speed_;
    };

    void Dog::Move(Direction direction, double speed) {

        if (direction != Direction::UNKNOWN) {
            downTime_ = std::chrono::milliseconds{0};    //сбрасываем врмя простоя
        }

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

    bool Dog::CheckFullBag() const{
        return bag_.size() >= bagSize_;
    }

    bool Dog::CheckEmptyBag() const{
        return bag_.size() == 0;
    }

    void Dog::ReturnLootInOffice() {
        for (auto obj : bag_) {
            AccumulateScore(obj->GetValue());
        }
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
    void Dog::AccumulateScore(size_t score) {
        score_ += score;
    }

    const size_t Dog::GetScore() const {
        return score_;
    }

    const size_t Dog::GetBagSize() const {
        return bagSize_;
    }

    void Dog::SetMaxStoppedTime(size_t time/*тут время в секундах!!!*/) {
        maxDownTime_ = std::chrono::seconds(time);
    };

    std::chrono::seconds  Dog::GetOnlineTime() {
        return std::chrono::duration_cast<std::chrono::seconds>(onlineTime_);
    }

    bool Dog::isDogSubjToDelete() {
        if ((activity_ == DogActivity::RUN) || (downTime_ < maxDownTime_)){
            return false;
        }

        return true;
    }

    void Dog::AccumulateOnlineTime(std::chrono::milliseconds time) {
        onlineTime_ += time;
    }

    void Dog::AccumulateDownTime(std::chrono::milliseconds time) {
        downTime_ += time;
    }

    DogActivity Dog::GetDogActivity() {
        return activity_;
    }

}