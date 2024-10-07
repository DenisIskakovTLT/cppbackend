#pragma once 
#include "player.h"
#include "dog_serialize.h"
#include "tokens.h"

#include <boost/serialization/vector.hpp>

namespace serialization {

	class PlayerRepr {
	public:

		PlayerRepr() = default;
		PlayerRepr(PlayerRepr&& player) = default;
		PlayerRepr(app::Player& player, const auth::Token& token)
		:id_(*player.GetId()),
		name_(player.GetName()),
		serializedDog_(*player.GetDog()),
		token_(*token)
		{

		};

		[[nodiscard]] app::Player RestoreIdName() const;
		[[nodiscard]] model::Dog RestoreDog() const;
		[[nodiscard]] auth::Token RestorePlayerToken() const;

		template <typename Archive>
		void serialize(Archive& ar, [[maybe_unused]] const unsigned version) {
			ar& id_;
			ar& name_;
			ar& serializedDog_;
			ar& token_;
		}

	private:
		size_t id_;
		std::string name_;
		DogRepr serializedDog_;
		std::string token_;
	};

}