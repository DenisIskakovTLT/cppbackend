#include "dog_gather.h"
#include <iostream>
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

        model::LostObject* tmpLostObj = NULL;
        try{
            tmpLostObj = dynamic_cast<model::LostObject*>(items_[idx].get());
        }
        catch (std::bad_cast ex) {
            std::cout << "Bad cast exception:" << ex.what();                //Что-то пошло не так с приведением типа
        }
        return tmpLostObj;
    };

}