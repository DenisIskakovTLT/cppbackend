#include "constdef.h"
#include "lost_obj.h"

namespace model {

    const LostObject::Id& LostObject::GetId() const {
        return id_;
    };

    size_t LostObject::GetValue() const {
        return value_;
    };

    void LostObject::SetValue(size_t value) {
        value_ = value;
    };

}