#pragma once

#include <string>
#include <vector>

namespace app {
    class PlayerDataForPostgres {
    public:
        PlayerDataForPostgres(std::string name, size_t score, int64_t playTime)
            :name_(name),
            score_(score),
            playTime_(playTime)
        {

        }

        const std::string& GetName() const noexcept {
            return name_;
        }

        size_t GetScore() const noexcept {
            return score_;
        }

        double GetPlayTime() const noexcept {
            return playTime_;
        }

    private:
        std::string name_ = "";
        size_t score_ = 0;
        int64_t playTime_ = 0.0;
    };

    class PlayerDataRepository {
    public:
        virtual void Save(const std::vector<PlayerDataForPostgres>& data) = 0;
        virtual std::vector<PlayerDataForPostgres> GetData(size_t offset, size_t limit) = 0;

    protected:
        ~PlayerDataRepository() = default;
    };

}
