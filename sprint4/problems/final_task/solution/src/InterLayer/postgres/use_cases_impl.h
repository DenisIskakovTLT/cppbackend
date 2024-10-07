#pragma once
#include "use_cases.h"

namespace postgres {

    class UseCasesImpl : public UseCases {
    public:
        explicit UseCasesImpl(app::PlayerDataRepository& data)
            : playerDataForPostgres_{ data } {
        }

        void Save(const std::vector<app::PlayerDataForPostgres>& data) override;
        std::vector<app::PlayerDataForPostgres> GetData(size_t offset, size_t limit) override;

    private:
        app::PlayerDataRepository& playerDataForPostgres_;
    };

}