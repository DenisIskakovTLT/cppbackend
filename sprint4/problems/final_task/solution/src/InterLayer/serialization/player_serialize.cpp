#include "player_serialize.h"

namespace serialization {

	[[nodiscard]] app::Player PlayerRepr::RestoreIdName() const {
		return app::Player(app::Player::Id{ id_ }, name_);
	}

	[[nodiscard]] model::Dog PlayerRepr::RestoreDog() const {
		return serializedDog_.Restore();
	}

	[[nodiscard]] auth::Token PlayerRepr::RestorePlayerToken() const {
		return auth::Token(token_);
	}

}