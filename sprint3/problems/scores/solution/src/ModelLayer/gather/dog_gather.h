#pragma once
#include "collision_detector.h"
#include "dog.h"
#include "lost_obj.h"
#include "map.h"

#include <unordered_map>
#include <memory>

namespace model {

	class DogGather : public collision_detector::ItemGathererProvider {
	public:
		using VectorItems = std::vector< std::shared_ptr<collision_detector::Item>>;
		using VectorDogs = std::vector< std::shared_ptr<Dog> >;

		DogGather(VectorItems&& items, VectorDogs&& dogs)
		:	items_(std::move(items)), 
			dogs_(std::move(dogs)){};

		/*Тут все перегрузки из ItemGathererProvider*/
		 size_t ItemsCount() const override;
		 collision_detector::Item GetItem(size_t idx) const override;
		 size_t GatherersCount() const override;
		 collision_detector::Gatherer GetGatherer(size_t idx) const override;

		 const Dog::Id& GetDogIdFromIdx(size_t idx) const;

		 const model::LostObject* GetLostObj(size_t idx) const;

		 /*Вирутальный деструктор*/
		 virtual ~DogGather() = default;

	private:
		VectorDogs dogs_;
		VectorItems items_;

	};

}