#pragma once
#include "constdef.h"
#include "geom.h"
#include "collision_detector.h"
#include "typedef.h"
#include "tagged.h"


namespace model {

	class LostObject : public collision_detector::Item {
	public:

		using Id = util::Tagged<size_t, LostObject>;

		LostObject(double width = LOOT_WIDTH) :
			id_(Id{ LostObject::start_id_++ }),
			Item({0, 0}, width)
		{};

		/*Сеттер и геттер позиции теперь берутся из базового класса Item*/

		/*Геттеры*/
		const Id& GetId() const;
		size_t GetValue() const;

		/*Сеттеры*/
		void SetValue(size_t value);
		void SetId(Id id);
	private:
		inline static size_t start_id_ = 0;		//С этого номера будут увеличиваться айдишники
		Id id_;									//Ид объекта
		size_t value_{ 0 };						//Для подсчёта очков
	};


	struct lostObjHasher {
		size_t operator()(const LostObject& obj) const {
			size_t randomNumber = 10;
			return std::hash<size_t>{}(obj.GetType()) * randomNumber +
				std::hash<double>{}(obj.GetPosition().x) * randomNumber +
				std::hash<double>{}(obj.GetPosition().y) * randomNumber;
		}
	};
}