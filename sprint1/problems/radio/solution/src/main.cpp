#include "audio.h"
#include <iostream>
#include <boost/asio.hpp>
#include <sstream>
#include <regex>

namespace net = boost::asio;
using net::ip::udp;
using namespace std::literals;

const size_t MAX_PORT = 65535;
static const size_t MAX_BUFF_SIZE = 65000;              //Максимальный размер буфера
static const int MAX_FRAMES_Q = 65000;                  //Максимальное кол-во фреймов

void PrintDebugErrorMessage(const char* taskName)
{
    std::cout << "Task name: "s << taskName << " port "s << " <connection port>"s << std::endl;
}

bool portIsValid(size_t port) {
    return port < MAX_PORT;
}

void StartServer(uint16_t port)
{
    try
    {
        boost::asio::io_context io_context;                                             //Контекст 

        udp::socket socket(io_context, udp::endpoint(udp::v4(), port));                 //Создаём сокет

        
        while(true)                                                                     //Запуск
        {
            
            std::array<char, MAX_BUFF_SIZE> receiveBuf;                                 //Входной буффер

            auto inputSize = socket.receive(boost::asio::buffer(receiveBuf));           //Принимаем

            Player player(ma_format_u8, 1);                                             //Проигрываетель

            size_t frames = inputSize / player.GetFrameSize();                          //Кол-во фреймов

            
            player.PlayBuffer(receiveBuf.data(), frames, 1.5s);                         //Воспроизведение
            std::cout << "Playing cmplt" << std::endl;
        }
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}

void StartClient(uint16_t port)
{
    static const char serverIP[] = "localhost";                                 //для отладки локалхост, но не плохо было бы принимать откуда-то

    Recorder recorder(ma_format_u8, 1);                                         //Рекордер

    std::string str;                                                            //Времянка для ввода строки
    std::cout << "Record message start..." << std::endl;
    std::getline(std::cin, str);

    // Производим запись в rec_result
    auto tmpRes = recorder.Record(MAX_FRAMES_Q, 1.5s);                          //Записываем рекордером
    std::cout << "Recording cmplt" << std::endl;

    size_t frames = tmpRes.frames;
    size_t outPackSize = recorder.GetFrameSize() * frames;
        
    if (outPackSize > MAX_BUFF_SIZE)                                            //Проверка, что попали в диапазон длины
    {
        std::cout << "Error: input size " << outPackSize << " more then max buffer size " << MAX_BUFF_SIZE << std::endl;
        return;
    }

    
    try
    {   
        net::io_context io_context;                                             //Контекст
        udp::socket socket(io_context, udp::v4());                              //Открываем сокет

        boost::system::error_code ec;                                           //Для кодов ошибок
        auto endpoint = udp::endpoint(net::ip::make_address(serverIP, ec), port);
        socket.send_to(net::buffer(tmpRes.data, outPackSize), endpoint);
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}

int main(int argc, char** argv)
{
    
    if (argc != 3)
    {
        PrintDebugErrorMessage(argv[0]);
        return 1;
    }
    std::string mode = std::string(argv[1]);

    if (mode != "server"s && mode != "client"s)
    {
        PrintDebugErrorMessage(argv[0]);
        return 1;
    }

    int port = 0;

    port = std::stoi(std::string(argv[2]));

    if (!portIsValid(port)) {
        std::cout << "Invalid port" << std::endl;
        std::exit(1);
    }

    if (mode == "client"s) //Клиент
    {
        StartClient(port);
    }

    if (mode == "server"s) //Сервер
    {
        StartServer(port);
    }

    return 0;
}