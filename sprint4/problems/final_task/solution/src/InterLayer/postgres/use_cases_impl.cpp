#include "use_cases_impl.h"

#include <ranges>
#include <algorithm>
#include <sstream>
#include <optional>

namespace postgres {

    void UseCasesImpl::Save(const std::vector<app::PlayerDataForPostgres>& data) {
        playerDataForPostgres_.Save(data);
    };

    std::vector<app::PlayerDataForPostgres>  UseCasesImpl::GetData(size_t offset, size_t limit) {
        return playerDataForPostgres_.GetData(offset, limit);
    };

}  // namespace app