#include "constdef.h"
#include "lost_obj.h"

namespace model {

    const LostObject::Id& LostObject::GetId() const {
        return id_;
    };

    size_t LostObject::GetType() const {
        return type_;
    };

    const Position& LostObject::GetPosition() const {
        return position_;
    };

    void LostObject::SetType(size_t type) {
        type_ = type;
    };

    void LostObject::SetPosition(Position position) {
        position_ = position;
    };

}