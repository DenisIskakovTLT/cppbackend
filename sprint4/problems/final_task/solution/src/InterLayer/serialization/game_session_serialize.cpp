#include "game_session_serialize.h"


namespace serialization {
	model::Map::Id GameSessionRepr::RestoreMapId() const {
		return model::Map::Id(mapId_);
	};

	const std::vector<PlayerRepr>& GameSessionRepr::GetSerializedPlayers() const {
		return serializedPlayers_;
	};

	const std::vector<LostObjRepr>& GameSessionRepr::GetSerializedLostObj() const {
		return serializedLostObjs_;
	};
}