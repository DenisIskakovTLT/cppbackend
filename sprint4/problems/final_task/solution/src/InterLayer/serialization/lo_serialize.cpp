#include "lo_serialize.h"


namespace serialization {

	model::LostObject LostObjRepr::Restore() const {
		model::LostObject tmpLostObj;
		tmpLostObj.SetId(model::LostObject::Id{ id_ });
		tmpLostObj.SetType(type_);
		tmpLostObj.SetPosition(position_);
		tmpLostObj.SetValue(value_);
		return tmpLostObj;
	}
}