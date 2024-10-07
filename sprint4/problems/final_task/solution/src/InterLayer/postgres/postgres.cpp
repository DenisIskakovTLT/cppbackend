#include "postgres.h"

#include <string>

#include <pqxx/pqxx>
#include <pqxx/zview.hxx>

namespace postgres {
	using namespace std::literals;
	using pqxx::operator"" _zv;

    Database::Database(const Connection::ConnectionConfig& connectionCfg)
        : connectionPool_{connectionCfg.connectionNum, [&connectionCfg]() {
                return std::make_shared<pqxx::connection>(connectionCfg.url);}
        } 
    {
        auto connection = connectionPool_.GetConnection();
        pqxx::work work{ *connection };
        /*https://metanit.com/sql/postgresql/2.3.php шпаргалка с типами данных*/
        work.exec(R"(
CREATE TABLE IF NOT EXISTS retired_players (
    id SERIAL PRIMARY KEY,
    name varchar(100) NOT NULL,
    score INTEGER NOT NULL,
    playTime INTEGER NOT NULL  
);

CREATE INDEX IF NOT EXISTS idx_score ON retired_players (score); 
)"_zv);
        
        work.commit();
    }


    void PlayerDataRepositoryImpl::Save(const std::vector<app::PlayerDataForPostgres>& data) {
        auto conn = connectionPool_.GetConnection();
        pqxx::work work{ *conn };
        for (const auto& plData : data) {
            /*https://metanit.com/sql/postgresql/3.1.php шпаргалка для INSERT*/
            work.exec_params(R"(
            INSERT INTO retired_players (name, score, playTime) VALUES ($1, $2, $3);
            )"_zv,
            plData.GetName(), plData.GetScore(), plData.GetPlayTime());
        }
        work.commit();
    };

    std::vector<app::PlayerDataForPostgres> PlayerDataRepositoryImpl::GetData(size_t offset, size_t limit) {

        std::vector<app::PlayerDataForPostgres> playersData;

        auto conn = connectionPool_.GetConnection();
        pqxx::read_transaction transact{ *conn };
        /*https://metanit.com/sql/postgresql/4.2.php шпаргалка для сортировки*/
        auto q = "SELECT name, score, playTime FROM retired_players ORDER BY score DESC, playTime ASC, name ASC LIMIT "
            + std::to_string(limit) + " OFFSET " + std::to_string(offset) + ";";

        for (auto [name, score, playTime] : transact.query<std::string, size_t, double>(q)) {
            playersData.emplace_back(name, score, playTime);
        }

        return playersData;
    };
}