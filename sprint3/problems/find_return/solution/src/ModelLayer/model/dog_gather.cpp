#include "dog_gather.h"

namespace model {

    size_t DogGather::ItemsCount() const {
        return items_.size();
    };

    collision_detector::Item DogGather::GetItem(size_t idx) const {
        return *items_[idx];
    };

    size_t DogGather::GatherersCount() const {
        return dogs_.size();
    };

    collision_detector::Gatherer DogGather::GetGatherer(size_t idx) const {
        return dogs_[idx]->GetGatherer();
    };

    const Dog::Id& DogGather::GetDogIdFromIdx(size_t idx) const {
        return dogs_[idx]->GetId();
    };

    const model::LostObject* DogGather::GetLostObj(size_t idx) const {
        return dynamic_cast<model::LostObject*>(items_[idx].get());
    };

}