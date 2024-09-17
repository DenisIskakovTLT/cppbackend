#pragma once
#include "constdef.h"
#include "typedef.h"

#include <boost/json.hpp>

namespace model {
	//Сюда надо перенести все tag invoke, для дальнейшего рефактора

	LootGenCfg tag_invoke(boost::json::value_to_tag<LootGenCfg>, const boost::json::value& jv);
	
	void tag_invoke(boost::json::value_from_tag, boost::json::value& jv, const LootType& lootType);
	LootType tag_invoke(boost::json::value_to_tag<LootType>, const boost::json::value& jv);
}