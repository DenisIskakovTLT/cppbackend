#include "application.h"

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include "player_serialize.h"

namespace app {

    using namespace std::literals;

    const model::Game::Maps& Application::ListMap() const noexcept {
        return game_.GetMaps();
    };

    const std::shared_ptr<model::Map> Application::FindMap(const model::Map::Id& id) const noexcept {
        return game_.FindMap(id);
    };

    std::tuple<auth::Token, Player::Id> Application::JoinGame(               //Если разрастётся, то придётся тапл на структуру поменять
        const std::string& name,
        const model::Map::Id& id) {

        auto player = CreatePlayer(name);
        auto token = playerTokens_.AddPlayer(player);

        auto session = GetGameSessionByMapId(id);
        if (!session) {
            session = std::make_shared<GameSession>(game_.FindMap(id), tickPeriod_, game_.GetLootGenConfig(), ioc_);
            AddGameSession(session.value());
            session.value()->StartGame();
        }
        BindPlayerInSession(player, session.value());
        sessionToTokenPlayer_[session.value()->GetId()][token] = player->GetId();
        return std::tie(token, player->GetId());
    };

    std::shared_ptr<Player> Application::CreatePlayer(const std::string& player_name) {
        auto player = std::make_shared<Player>(player_name);
        players_[player->GetId()] = player;
        return player;
    };

    void Application::BindPlayerInSession(std::shared_ptr<Player> player,
        std::shared_ptr<GameSession> session) {
        player->SetGameSession(session);
        auto tmpDog = session->MakeDog(player->GetName(), *(session->GetMap()), randomizePosition_);
        player->SetDog(tmpDog);
    };

    std::vector< std::shared_ptr<Player>> Application::GetPlayersFromSession(auth::Token token) {
        std::vector< std::shared_ptr<Player> > emptyPlayerList;
        auto player = playerTokens_.FindPlayerByToken(token).lock();
        if (!player) {
            return emptyPlayerList;
        }
        auto playerSession = player->GetSessionId();
        if (!sessionToTokenPlayer_.contains(playerSession)) {
            return emptyPlayerList;
        }

        for (const auto& [token, playerId] : sessionToTokenPlayer_[playerSession]) {
            if (players_.contains(playerId)) {
                emptyPlayerList.push_back(players_[playerId]);
            }
        }

        return emptyPlayerList;
    };

    bool Application::CheckPlayerByToken(auth::Token token) {
        return !playerTokens_.FindPlayerByToken(token).expired();
    };

    /*Сюда попадаем, только если ручное управление временем.
    При автоматическом управлении апдейтер будет вызывать тикер игры, а сохранения тикер сохранений*/
    void Application::UpdateGameState(const std::chrono::milliseconds& time) {

        for (auto [id, session] : sessions_) {

            session->UpdateGameState(time);

        }

        SaveGamePeriodically(time);
    };

    void Application::MovePlayer(const auth::Token& token, model::Direction direction) {
        auto player = playerTokens_.FindPlayerByToken(token).lock();
        if (!player) {
            return;
        }
        auto dog = player->GetDog();
        double speed = player->GetSession()->GetMap()->GetDogSpeed();
        dog->Move(direction, speed);
    };

    bool Application::CheckTimeManage() const {
        return tickPeriod_.count() == 0;
    };

    void Application::AddGameSession(std::shared_ptr<GameSession> session) {

        if (auto [it, inserted] = mapIdToSessionIndex_.emplace(session->GetMap()->GetId(), session->GetId()); !inserted) {
            throw std::invalid_argument("Game session with map id "s + *(session->GetMap()->GetId()) + " already exists"s);
        }
        else {
            try {
                sessions_[session->GetId()] = session;
            }
            catch (...) {
                mapIdToSessionIndex_.erase(it);
                throw;
            }
        }

        /*Для паттерна "Наблюдатель" передадим линк на функции в GameSession из текущего класса*/
        auto SaveLambda = [self = shared_from_this()](const std::vector<app::PlayerDataForPostgres>& data) {
            self->SavePlayerDataInDB(data);
            };
        session->SetSaveDataToPostgresFunctional(SaveLambda);

        auto DeleteLambda = [self = shared_from_this()](const GameSession::Id& id) {
            self->DeleteAFKPlayers(id);
            };
        session->SetDeleteAFKPlayersFuntional(DeleteLambda);
    };

    std::optional<std::shared_ptr<GameSession>> Application::GetGameSessionByMapId(const model::Map::Id& id) const noexcept {
        if (auto it = mapIdToSessionIndex_.find(id); it != mapIdToSessionIndex_.end()) {
            return sessions_.at(it->second);
        }
        return std::nullopt;
    };

    std::optional<std::shared_ptr<GameSession>> Application::GetGameSessionByToken(const auth::Token& token) const noexcept {
        for (const auto& [id, tokPlay] : sessionToTokenPlayer_) {
            if (tokPlay.contains(token)) {
                return sessions_.at(id);
            }
        }
        return std::nullopt;
    }

    void Application::SaveGame() {

        if (!savedParameters_.save_file_path.has_value()) {
            return; //Путь пуст, некуда сериализировать
        }

        std::vector<serialization::GameSessionRepr> serializedSessions = SerializeGame();
        std::fstream outputFile;
        outputFile.open(savedParameters_.save_file_path.value(), std::ios_base::out);
        {
            boost::archive::text_oarchive oarchive{ outputFile };
            oarchive << serializedSessions;
        }

    }

    std::vector<serialization::GameSessionRepr> Application::SerializeGame() {

        std::vector<serialization::GameSessionRepr> serializedSession_;        //Сюда будем наполнять сериализированные данные
        for (const auto& [id, session] : sessions_) {
            std::unordered_map < auth::Token, std::shared_ptr<app::Player>, auth::TokenHasher> tokenPlayer;

            for (const auto& [token, plId] : sessionToTokenPlayer_.at(session->GetId())) {
                auto player = players_.at(plId);
                tokenPlayer.insert(std::make_pair(token, player));
            }
            serializedSession_.push_back(serialization::GameSessionRepr(*session, tokenPlayer));
        }
        return serializedSession_;
    }

    void Application::LoadGameFromSave(savegame::SavedFileParameters parameters) {

        savedParameters_ = parameters;

        /*Тут загрузка параметров, десериализация из файла*/
        DeserializationGameState();

        if (!(savedParameters_.save_file_path.has_value() && savedParameters_.saved_tick_period.has_value())
            || CheckTimeManage()) {
            return;  //Переданы пустые параметры, ни файла ни периода или ручное управление временем. Уходим.
        }

        /*Включим тикер на сохранения*/
        StartSaveTicker();
        //Тикер используется, только при автоматическом управлении временем.
        //При ручном управлении временем используется SaveGamePeriodically
    }

    void Application::StartSaveTicker() {
        saveTicker_ = std::make_shared<tickerTime::Ticker>(
            ioc_,
            savedParameters_.saved_tick_period.value(),
            [self = shared_from_this()](const std::chrono::milliseconds& time) {
                self->SaveGame();
            });

        saveTicker_->Start();
    }

    void Application::DeserializationGameState() {

        if (!savedParameters_.save_file_path.has_value()) {
            return; //Путь пуст, нечего десереализировать
        }

        std::vector<serialization::GameSessionRepr> serializedSession_;        //Сюда считаем сериализованные сессии
        std::fstream inputFile;
        inputFile.open(savedParameters_.save_file_path.value(), std::ios_base::in);

        if (!inputFile.is_open()) {
            return;             //файл не открылся, значит его нет, выходим
        }

        boost::archive::text_iarchive iarchive{ inputFile };
        iarchive >> serializedSession_;                                         //Считали сессии
        inputFile.close();                                                      //Закрыли файл

        for (const serialization::GameSessionRepr& session : serializedSession_) {  //Идём по сессиям и парсим все данные по параметрам
            std::shared_ptr<GameSession> tmpGameSession = std::make_shared<GameSession>(
                game_.FindMap(session.RestoreMapId()),
                tickPeriod_,
                game_.GetLootGenConfig(),
                ioc_
            );

            for (const auto& player : session.GetSerializedPlayers()) {         //Парсим игроков
                auto tmpDog = std::make_shared<model::Dog>(std::move(player.RestoreDog()));
                auto tmpPlayer = std::make_shared<app::Player>(std::move(player.RestoreIdName()));
                auto tmpToken = player.RestorePlayerToken();
                tmpPlayer->SetDog(tmpDog);
                tmpPlayer->SetGameSession(tmpGameSession);
                tmpGameSession->SetDog(tmpDog);
                sessionToTokenPlayer_[tmpGameSession->GetId()][tmpToken] = tmpPlayer->GetId();
                playerTokens_.SetPlayerToken(tmpToken, tmpPlayer);

            }

            for (const auto& lostObj : session.GetSerializedLostObj()) {        //Парсим лут
                tmpGameSession->SetLostObject(lostObj.Restore());
            }

            /*Добавляем сессию и стартуем её*/
            AddGameSession(tmpGameSession);
            tmpGameSession->StartGame();
        }
    }

    void Application::SaveGamePeriodically(const std::chrono::milliseconds& time) {

        if (!savedParameters_.saved_tick_period.has_value()) {
            return;
        }

        static int saveCount = 0;
        if (savedParameters_.saved_tick_period) {
            saveCount = savedParameters_.saved_tick_period.value().count();
        }

        saveCount -= time.count();
        if (saveCount <= 0) {
            SaveGame();
            saveCount = savedParameters_.saved_tick_period.value().count();
        }
    }

    void Application::SavePlayerDataInDB(std::vector<PlayerDataForPostgres> data) {
        use_cases_.Save(data);
    }

    void Application::DeleteAFKPlayers(const GameSession::Id& input_id) {
        //https://habr.com/ru/companies/piter/articles/706866/ шпаргалка по умным указателям
        TokenPlayer tmpPlayersForDelete;

        for (const auto& [token, id] : sessionToTokenPlayer_.at(input_id)) {
            if (sessions_.at(input_id)->GetDogs().empty()) {
                tmpPlayersForDelete.insert(std::make_pair(token, id));
            }
        }

        auto deletePredicat = [&tmpPlayersForDelete](const auto& tokenPlrs) { return tmpPlayersForDelete.contains(tokenPlrs.first); };
        std::erase_if(sessionToTokenPlayer_.at(input_id), deletePredicat);

        for (const auto& [token, id] : tmpPlayersForDelete) {
            players_.erase(id);
        }

    }

    std::optional<std::vector<app::PlayerDataForPostgres>> Application::GetRecords(
        std::optional<size_t> offset, std::optional<size_t> limit) {
        size_t startIdx{ 0 };
        size_t outputLimit{ model::DEFAULT_OUTPUT_LIMIT };

        if (offset.has_value()) {
            startIdx = offset.value();
        }
        if (limit.has_value()) {
            if (limit.value() > model::DEFAULT_OUTPUT_LIMIT) {
                return std::nullopt;
            }
            outputLimit = limit.value();
        }
        return use_cases_.GetData(startIdx, outputLimit);
    };

}