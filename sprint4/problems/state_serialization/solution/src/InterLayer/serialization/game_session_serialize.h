#pragma once 

#include <vector>
#include <boost/serialization/vector.hpp>

#include "game_session.h"
#include "lo_serialize.h"
#include "map.h"
#include "player_serialize.h"
#include "typedef.h"


namespace serialization {

	class GameSessionRepr {

	using TokenPlayer = std::unordered_map< auth::Token, std::shared_ptr<app::Player>, auth::TokenHasher>;

	public:
		GameSessionRepr() = default;
		GameSessionRepr(app::GameSession& session, TokenPlayer tokenPlayer)
			:mapId_(*session.GetMap()->GetId())
		{
			/*Приведение типа, аналогично как в DogRepr*/
			auto ConvertPlayerToSerializedPlayer = [](const auto& tokenPlayerInput)->PlayerRepr {
				return PlayerRepr(*tokenPlayerInput.second, tokenPlayerInput.first);
				};


			auto ConvertLostObjToSerializedLostObj = [](const auto& idLostObjInput)->LostObjRepr {
				return *idLostObjInput.second;
				};

			std::ranges::transform(tokenPlayer, std::back_inserter(serializedPlayers_), ConvertPlayerToSerializedPlayer);
			std::ranges::transform(session.GetLostObj(), std::back_inserter(serializedLostObjs_), ConvertLostObjToSerializedLostObj);


		};

		[[nodiscard]] model::Map::Id RestoreMapId() const;
		[[nodiscard]] const std::vector<PlayerRepr>& GetSerializedPlayers() const;
		[[nodiscard]] const std::vector<LostObjRepr>& GetSerializedLostObj() const;
		

		template <typename Archive>
		void serialize(Archive& ar, [[maybe_unused]] const unsigned version) {
			ar& mapId_;
			ar& serializedPlayers_;
			ar& serializedLostObjs_;
		}
	private:
		std::string mapId_;
		std::vector<PlayerRepr> serializedPlayers_;
		std::vector<LostObjRepr> serializedLostObjs_;
	};

}