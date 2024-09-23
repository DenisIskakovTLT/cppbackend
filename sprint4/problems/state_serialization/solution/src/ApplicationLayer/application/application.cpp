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

        std::shared_ptr<GameSession> session = GetGameSessionByMapId(id);
        if (!session) {
            session = std::make_shared<GameSession>(game_.FindMap(id), tickPeriod_, game_.GetLootGenConfig(), ioc_);
            AddGameSession(session);
            session->StartGame();
        }
        tokenToSessionIndex_[token] = session;
        sessionToTokenPlayer_[session][token] = player;
        BindPlayerInSession(player, session);
        return std::tie(token, player->GetId());
    };

    std::shared_ptr<Player> Application::CreatePlayer(const std::string& player_name) {
        auto player = std::make_shared<Player>(player_name);
        players_.push_back(player);
        return player;
    };

    void Application::BindPlayerInSession(std::shared_ptr<Player> player,
        std::shared_ptr<GameSession> session) {
        sessionID_[session->GetId()].push_back(player);
        player->SetGameSession(session);
        player->SetDog(player->GetName(), *(session->GetMap()), randomizePosition_);
    };

    const std::vector< std::weak_ptr<Player> >& Application::GetPlayersFromSession(auth::Token token) {
        static const std::vector< std::weak_ptr<Player> > emptyPlayerList;
        auto player = playerTokens_.FindPlayerByToken(token).lock();
        auto session_id = player->GetSessionId();
        if (!sessionID_.contains(session_id)) {
            return emptyPlayerList;
        }
        return sessionID_[session_id];
    };

    bool Application::CheckPlayerByToken(auth::Token token) {
        return !playerTokens_.FindPlayerByToken(token).expired();
    };

    /*Сюда попадаем, только если ручное управление временем*/
    void Application::UpdateGameState(const std::chrono::milliseconds& time) {

        for (auto session: sessions_) {
            session->UpdateGameState(time);
        }

        SaveGamePeriodically(time);
    };

    void Application::MovePlayer(const auth::Token& token, model::Direction direction) {
        auto player = playerTokens_.FindPlayerByToken(token).lock();
        auto dog = player->GetDog();
        double speed = player->GetSession()->GetMap()->GetDogSpeed();
        dog->Move(direction, speed);
    };

    bool Application::CheckTimeManage() const{
        return tickPeriod_.count() == 0;
    };

    void Application::AddGameSession(std::shared_ptr<GameSession> session) {
        const size_t index = sessions_.size();
        if (auto [it, inserted] = mapIdToSessionIndex_.emplace(session->GetMap()->GetId(), index); !inserted) {
            throw std::invalid_argument("Game session with map id "s + *(session->GetMap()->GetId()) + " already exists"s);
        }
        else {
            try {
                sessions_.push_back(session);
            }
            catch (...) {
                mapIdToSessionIndex_.erase(it);
                throw;
            }
        }
    };

    std::shared_ptr<GameSession> Application::GetGameSessionByMapId(const model::Map::Id& id) const noexcept {
        if (auto it = mapIdToSessionIndex_.find(id); it != mapIdToSessionIndex_.end()) {
            return sessions_.at(it->second);
        }
        return nullptr;
    };

    std::shared_ptr<GameSession> Application::GetGameSessionByToken(const auth::Token& token) const noexcept {
        if (auto it = tokenToSessionIndex_.find(token); it != tokenToSessionIndex_.end()) {
            return it->second;
        }
        return nullptr;
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

        for(auto session : sessions_){
            serializedSession_.push_back(serialization::GameSessionRepr(*session, sessionToTokenPlayer_.at(session)));
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
                tokenToSessionIndex_[tmpToken] = tmpGameSession;
                sessionToTokenPlayer_[tmpGameSession][tmpToken] = tmpPlayer;
                playerTokens_.SetPlayerToken(tmpToken, tmpPlayer);
                sessionID_[tmpGameSession->GetId()].push_back(tmpPlayer);

            }

            for (const auto& lostObj : session.GetSerializedLostObj()) {        //Парсим лут
                tmpGameSession->SetLostObject(lostObj.Restore());
            }

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
}