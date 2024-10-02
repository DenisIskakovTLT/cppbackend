#pragma once 
#include "geom.h"
#include "lost_obj.h"

#include <boost/serialization/vector.hpp>

namespace serialization {

	class LostObjRepr {
	public:
		LostObjRepr() = default;

		LostObjRepr(const model::LostObject& obj)
			:id_(*obj.GetId()),
			value_(obj.GetValue()),
			position_(obj.GetPosition()),
			type_(obj.GetType())
		{

		};

		[[nodiscard]] model::LostObject Restore() const;

		template <typename Archive>
		void serialize(Archive& ar, [[maybe_unused]] const unsigned version) {
			ar& id_;
			ar& value_;
			ar& position_;
			ar& type_;
		}
	private:
		size_t id_;
		size_t value_;
		geom::Point2D position_;
		size_t type_;
	};

}