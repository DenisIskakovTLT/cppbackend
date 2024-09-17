#include "constdef.h"
#include "lost_obj.h"

namespace model {

    const LostObject::Id& LostObject::GetId() const {
        return id_;
    };

}