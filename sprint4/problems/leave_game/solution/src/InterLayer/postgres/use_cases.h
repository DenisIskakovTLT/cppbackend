#pragma once
#include "player_data_record.h"

#include <string>
#include <vector>

namespace postgres {

    class UseCases {
    public:
        virtual void Save(const std::vector<app::PlayerDataForPostgres>& data) = 0;
        virtual std::vector<app::PlayerDataForPostgres> GetData(size_t offset, size_t limit) = 0;

    protected:
        ~UseCases() = default;
    };

}