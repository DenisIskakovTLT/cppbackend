#ifdef WIN32
#include <sdkddkver.h>
#endif

#include "seabattle.h"

#include <atomic>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <iostream>
#include <optional>
#include <string>
#include <thread>
#include <string_view>

namespace net = boost::asio;
using net::ip::tcp;
using namespace std::literals;

void PrintFieldPair(const SeabattleField& left, const SeabattleField& right) {
    auto left_pad = "  "s;
    auto delimeter = "    "s;
    std::cout << left_pad;
    SeabattleField::PrintDigitLine(std::cout);
    std::cout << delimeter;
    SeabattleField::PrintDigitLine(std::cout);
    std::cout << std::endl;
    for (size_t i = 0; i < SeabattleField::field_size; ++i) {
        std::cout << left_pad;
        left.PrintLine(std::cout, i);
        std::cout << delimeter;
        right.PrintLine(std::cout, i);
        std::cout << std::endl;
    }
    std::cout << left_pad;
    SeabattleField::PrintDigitLine(std::cout);
    std::cout << delimeter;
    SeabattleField::PrintDigitLine(std::cout);
    std::cout << std::endl;
}

template <size_t sz>
static std::optional<std::string> ReadExact(tcp::socket& socket) {
    boost::array<char, sz> buf;
    boost::system::error_code ec;

    net::read(socket, net::buffer(buf), net::transfer_exactly(sz), ec);

    if (ec) {
        return std::nullopt;
    }

    return { {buf.data(), sz} };
}

static bool WriteExact(tcp::socket& socket, std::string_view data) {
    boost::system::error_code ec;

    net::write(socket, net::buffer(data), net::transfer_exactly(data.size()), ec);

    return !ec;
}

class SeabattleAgent {
public:
    SeabattleAgent(const SeabattleField& field)
        : my_field_(field) {
    }

    void StartGame(tcp::socket& socket, bool my_initiative) {
        // TODO: реализуйте самостоятельно
        while (!IsGameEnded()) {

            PrintFields();

            if (my_initiative)                                  //мой ход или нет.
            {

                std::cout << "Your turn: ";                     //Мой ход
                                
                std::string currentShot;                        //Ждём ввода из консоли координаты для выстрела
                std::cin >> currentShot;

                auto cellToShot = ParseMove(currentShot);       //Парсим методом, то что считали с консоли. Получим координату  парой int int

                if (!cellToShot) {                              //Ввели кривые координаты
                    std::cout << "Invalid coordinate. Try again." << std::endl;
                    continue;
                }

                
                makeMove(socket, *cellToShot);                                      //Совершаем выстрел и отправка инфы сопернику

                SeabattleField::ShotResult resFromEnemy = getResult(socket);            //Получаем инфу от соперника

                std::pair<int, int> cellInfo = { (*cellToShot).second, (*cellToShot).first };   //для вывода инфы

                
                if (resFromEnemy == SeabattleField::ShotResult::MISS) {
                    other_field_.MarkMiss(cellInfo.first, cellInfo.second);
                    my_initiative = false;  // Промазнулись, переход хода
                    std::cout << "MISS. The move goes to the enemy." << std::endl;
                }
                else if (resFromEnemy == SeabattleField::ShotResult::KILL) {        //Убил корабль до конца
                    other_field_.MarkKill(cellInfo.first, cellInfo.second);
                    std::cout << "KILL! Next move is your's" << std::endl;
                }
                else if (resFromEnemy == SeabattleField::ShotResult::HIT) {          //Попал, но не убил.
                    other_field_.MarkHit(cellInfo.first, cellInfo.second);
                    std::cout << "HIT! Next move is your's." << std::endl;
                }


            }
            else {                                              //Не мой ход

                std::cout << "Waiting for the opponent's move..."s << std::endl;

                auto resFromEnemy = moveFromOponent(socket);

                
                if (resFromEnemy) {                                                 //Пришли невалидные данные

                     ;
                    std::cout << "Shot to "s << MoveToString(*resFromEnemy) << std::endl;               //вывод хода соперника

                    std::pair<int, int> cellInfo = { (*resFromEnemy).second, (*resFromEnemy).first }; //для вывода инфы
                    auto shoot = my_field_.Shoot(cellInfo.first, cellInfo.second);


                    // Обработка результата
                    if (shoot == SeabattleField::ShotResult::MISS) {
                        my_field_.MarkMiss(cellInfo.first, cellInfo.second);
                        my_initiative = true;  // Соперник промазал - получаем право хода при промахе
                        std::cout << "Enemy MISS!  Next move is your."s << std::endl;
                    }
                    else if (shoot == SeabattleField::ShotResult::KILL) {
                        my_field_.MarkKill(cellInfo.first, cellInfo.second);
                        std::cout << "KILLED! The move goes to the enemy."s << std::endl;
                    }
                    else if (shoot == SeabattleField::ShotResult::HIT) {
                        my_field_.MarkHit(cellInfo.first, cellInfo.second);
                        std::cout << " HITED! The move goes to the enemy."s << std::endl;
                    }

                    // Отсылка результата
                    sendResultToOpenent(socket, shoot);
                }
                else
                {
                    std::cout << "Conection problem"s << std::endl;
                    break;
                }


            }

        }
    }

private:
    static std::optional<std::pair<int, int>> ParseMove(const std::string_view& sv) {
        if (sv.size() != 2) return std::nullopt;

        int p1 = sv[0] - 'A', p2 = sv[1] - '1';

        if (p1 < 0 || p1 > 8) return std::nullopt;
        if (p2 < 0 || p2 > 8) return std::nullopt;

        return { {p1, p2} };
    }

    static std::string MoveToString(std::pair<int, int> move) {
        char buff[] = { static_cast<char>(move.first) + 'A', static_cast<char>(move.second) + '1' };
        return { buff, 2 };
    }

    void PrintFields() const {
        PrintFieldPair(my_field_, other_field_);
    }

    bool IsGameEnded() const {
        return my_field_.IsLoser() || other_field_.IsLoser();
    }

    // TODO: добавьте методы по вашему желанию
    void makeMove(tcp::socket& socket, std::pair<int, int>& crd) {
        auto currMove = MoveToString(crd);
        std::string_view currMove_strv(currMove.c_str(), 2);
        WriteExact(socket, currMove_strv);
    }

    SeabattleField::ShotResult getResult(tcp::socket& socket) {
        auto fromOpponent = ReadExact<1>(socket);
        SeabattleField::ShotResult shotRes = static_cast<SeabattleField::ShotResult>((*fromOpponent)[0]);
        return shotRes;
    }

    std::optional<std::pair<int, int>> moveFromOponent(tcp::socket& socket) {
        auto fromOpponent = ReadExact<2>(socket);
        return ParseMove(*fromOpponent);
    }

    void sendResultToOpenent(tcp::socket& socket, SeabattleField::ShotResult shot_res) {
        char shot = static_cast<char>(shot_res);
        std::string_view currMove_strv(&shot, 1);
        WriteExact(socket, currMove_strv);
    }

private:
    SeabattleField my_field_;
    SeabattleField other_field_;
};

void StartServer(const SeabattleField& field, unsigned short port) {
    SeabattleAgent agent(field);

    net::io_context io_context;

    tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), port));
    std::cout << "Waiting opponent connection..."s << std::endl;

    boost::system::error_code ec;
    tcp::socket socket{ io_context };
    acceptor.accept(socket, ec);

    if (ec) {
        std::cout << "Connection problem"s << std::endl;
        return;
    }

    agent.StartGame(socket, false);
};

void StartClient(const SeabattleField& field, const std::string& ip_str, unsigned short port) {
    SeabattleAgent agent(field);

    boost::system::error_code ec;
    auto endpoint = tcp::endpoint(net::ip::make_address(ip_str, ec), port);     //'эндпоинт

    if (ec) {
        std::cout << "Invalid IP"s << std::endl;
        return;
    }

    net::io_context io_context;
    tcp::socket socket{ io_context };
    socket.connect(endpoint, ec);

    if (ec)
    {
        std::cout << "Connection problem"s << std::endl;
        return;
    }

    agent.StartGame(socket, true);
};

int main(int argc, const char** argv) {
    if (argc != 3 && argc != 4) {
        std::cout << "Usage: program <seed> [<ip>] <port>" << std::endl;
        return 1;
    }

    std::mt19937 engine(std::stoi(argv[1]));
    SeabattleField fieldL = SeabattleField::GetRandomField(engine);

    if (argc == 3) {
        StartServer(fieldL, std::stoi(argv[2]));
    }
    else if (argc == 4) {
        StartClient(fieldL, argv[2], std::stoi(argv[3]));
    }
}
