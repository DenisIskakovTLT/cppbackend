#include "constdef.h"
#include "tag_invoke_handler.h"

namespace model {

    LootGenCfg tag_invoke(boost::json::value_to_tag<LootGenCfg>, const boost::json::value& jv) {
        LootGenCfg tmpCfg;
        tmpCfg.period = boost::json::value_to<double>(jv.as_object().at(LOOT_GENERATOR_PERIOD));
        tmpCfg.probability = boost::json::value_to<double>(jv.as_object().at(LOOT_GENERATOR_PROBABILITY));
        return tmpCfg;
    };

    void tag_invoke(boost::json::value_from_tag, boost::json::value& jv, const LootType& lootType) {
        boost::json::object res = {
            {LOOT_TYPE_NAME, boost::json::value_from(lootType.name)},
            {LOOT_TYPE_FILE, boost::json::value_from(lootType.file)},
            {LOOT_TYPE_TYPE, boost::json::value_from(lootType.type)},
            {LOOT_TYPE_SCALE, boost::json::value_from(lootType.scale)},
            {LOOT_TYPE_VALUE, boost::json::value_from(lootType.value)}
        };
        if (!lootType.color.empty()) {
            res[LOOT_TYPE_COLOR] = boost::json::value_from(lootType.color);
        }
        if (lootType.rotation != INT_MIN) {
            res[LOOT_TYPE_ROTATION] = boost::json::value_from(lootType.rotation);
        }
        jv.emplace_object() = res;
    };

    LootType tag_invoke(boost::json::value_to_tag<LootType>, const boost::json::value& jv) {
        LootType loot_type;
        loot_type.name = boost::json::value_to<std::string>(jv.as_object().at(LOOT_TYPE_NAME));
        loot_type.file = boost::json::value_to<std::string>(jv.as_object().at(LOOT_TYPE_FILE));
        loot_type.type = boost::json::value_to<std::string>(jv.as_object().at(LOOT_TYPE_TYPE));
        loot_type.value = boost::json::value_to<size_t>(jv.as_object().at(LOOT_TYPE_VALUE));
        if (jv.as_object().contains(LOOT_TYPE_ROTATION)) {
            loot_type.rotation = boost::json::value_to<int>(jv.as_object().at(LOOT_TYPE_ROTATION));
        }
        if (jv.as_object().contains(LOOT_TYPE_COLOR)) {
            loot_type.color = boost::json::value_to<std::string>(jv.as_object().at(LOOT_TYPE_COLOR));
        }
        loot_type.scale = boost::json::value_to<double>(jv.as_object().at(LOOT_TYPE_SCALE));
        return loot_type;
    };

}