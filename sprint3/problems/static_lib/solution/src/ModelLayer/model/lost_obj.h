#pragma once

#include "typedef.h"
#include "tagged.h"


namespace model {

	class LostObject {
	public:

		using Id = util::Tagged<size_t, LostObject>;

		LostObject() :
			id_(Id{ LostObject::start_id_++ }),
			position_({0, 0}),
			type_(0)
		{};

		/*Геттеры*/
		const Id& GetId() const;
		size_t GetType() const;
		const Position& GetPosition() const;

		/*Сеттеры*/
		void SetType(size_t type);
		void SetPosition(Position position);

	private:
		inline static size_t start_id_ = 0;		//С этого номера будут увеличиваться айдишники
		Id id_;									//Ид объекта
		size_t type_;							//Тип
		Position position_;						//Позиция
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