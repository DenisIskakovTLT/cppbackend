#pragma once

#include <chrono>
#include <iomanip>
#include <fstream>
#include <iterator>
#include <sstream>
#include <string>
#include <string_view>
#include <optional>
#include <mutex>
#include <thread>

using namespace std::literals;

#define LOG(...) Logger::GetInstance().Log(__VA_ARGS__)

class Logger {

    auto GetTime() const {
        if (manual_ts_) {
            return *manual_ts_;
        }

        return std::chrono::system_clock::now();
    }

    auto GetTimeStamp() const {
        const auto now = GetTime();
        const auto t_c = std::chrono::system_clock::to_time_t(now);
        return std::put_time(std::gmtime(&t_c), "%F %T");
    }

    std::string GetFileTimeStamp() const {
        const auto now = GetTime();
        const auto t_c = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::gmtime(&t_c), "%Y_%m_%d");
        return ss.str();
    }

    template<typename T, class... Ts>
    void Log(std::stringstream& ss, T value, const Ts&... args) {           //Рекурсивыный вывод
        ss << value;
        Log(ss, args...);
    }

    void Log(std::stringstream& ss) {
    }

    Logger() = default;
    Logger(const Logger&) = delete;

public:
    static Logger& GetInstance() {
        static Logger obj;
        return obj;
    }

    // Выведите в поток все аргументы.
    template<class... Ts>
    void Log(const Ts&... args) {
        
        std::stringstream ss;                               //Строковый поток
        
        ss << GetTimeStamp() << ": ";                       //Тайм стамп выводим
        Log(ss, args...);                                   //Вызываем самого себя, рекурсивно
        
        std::string logDir("/var/log/sample_log_");         //Директрия
        std::string extention(".log");                      //Расширение
        const std::lock_guard<std::mutex> lock(fileM_);     //Захват мьютекса
        std::ofstream log_file_{ logDir + GetFileTimeStamp() + extention, std::ios::app };      //открыли файл
        log_file_ << ss.str() << std::endl;                 //вывалили в файл всё из потока
    }

    // Установите manual_ts_. Учтите, что эта операция может выполняться
    // параллельно с выводом в поток, вам нужно предусмотреть 
    // синхронизацию.
    void SetTimestamp(std::chrono::system_clock::time_point ts) {
        const std::lock_guard<std::mutex> lock(tsM__);
        manual_ts_ = ts;
    }

private:

    std::optional<std::chrono::system_clock::time_point> manual_ts_;
    std::mutex tsM__;													//Мьютекс для ts
    std::mutex fileM_; 												    //Мьютекс для файла
};