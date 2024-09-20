#include "dog_serialize.h"


namespace serialization {

	model::Dog DogRepr::Restore() const {
		model::Dog tmpDog(model::Dog::Id{id_}, name_, bagSize_);
		tmpDog.SetPosition(position_);
		tmpDog.SetDirection(direction_);

		for (const auto& obj : bag_) {
			tmpDog.PickUpLoot(std::make_shared<model::LostObject>(obj.Restore()));
		}

		return tmpDog;
	}
}