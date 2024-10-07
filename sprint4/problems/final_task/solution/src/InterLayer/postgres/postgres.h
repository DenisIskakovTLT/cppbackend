#pragma once
#include "connection_pool.h"
#include "player.h"
#include <pqxx/connection>
#include <pqxx/transaction>

namespace postgres {


    class PlayerDataRepositoryImpl : public app::PlayerDataRepository {
    public:
        explicit PlayerDataRepositoryImpl(Connection::ConnectionPool& pool)
            : connectionPool_(pool) {};

        void Save(const std::vector<app::PlayerDataForPostgres>& data) override;
        std::vector<app::PlayerDataForPostgres> GetData(size_t offset, size_t limit) override;

    private:
        Connection::ConnectionPool& connectionPool_;
    };

    class Database {
    public:
        /*Тут надо будет из пула полуичть соединение и создать таблицу если её нет*/
        explicit Database(const Connection::ConnectionConfig& connectionCfg); 

        PlayerDataRepositoryImpl& GetDataFromImpl()& {
            return dataForPostgres_;
        }

    private:
        Connection::ConnectionPool connectionPool_;
        PlayerDataRepositoryImpl dataForPostgres_{ connectionPool_ };
    };

}