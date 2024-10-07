#pragma once

#include <chrono>
#include <climits>
#include <string>
#include <optional>
#include <unordered_map>

namespace model {

    using Dimension = int;
    using Coord = Dimension;
    
    struct Point {
        Coord x, y;
    };

    struct Size {
        Dimension width, height;
    };

    struct Rectangle {
        Point position;
        Size size;
    };

    struct Offset {
        Dimension dx, dy;
    };

    enum class DogActivity { 
        RUN, 
        STOP 
    };

    enum class Direction {
        NORTH,
        SOUTH,
        WEST,
        EAST,
        UNKNOWN
    };

    struct Speed {
        double vx, vy;
    };

    struct LootGenCfg {
        double period, probability;
    };

    struct LootType {
        std::string name{ "" };
        std::string file{ "" };
        std::string type{ "" };
        /*-2147483648 (INT_MIN) по умолчанию, чтобы при отсутствие в конфигурации этого поля,
        в респонсах это поле просто игнорировалось, а не выводилось со значением 0
        */
        int rotation{ INT_MIN };      
        std::string color{ "" };
        double scale{ 0.0 };
        size_t value{ 0 };
    };

    bool operator == (const Speed& lhs, const Speed& rhs);

    struct SpeedHasher {
        size_t operator()(const Speed& spd) const {
            size_t sd = 17;
            return std::hash<double>{}(spd.vy)* sd + std::hash<double>{}(spd.vx);
        }
    };

    const std::unordered_map<Direction, std::string> DIRECTION_TO_JSON = {
    {Direction::NORTH, "U"},
    {Direction::SOUTH, "D"},
    {Direction::WEST,  "L"},
    {Direction::EAST,  "R"},
    {Direction::UNKNOWN, ""}
    };

    const std::unordered_map<std::string, Direction> JSON_TO_DIRECTION = {
    {"U", Direction::NORTH},
    {"D", Direction::SOUTH},
    {"L", Direction::WEST},
    {"R", Direction::EAST},
    {"", Direction::UNKNOWN}
    };

    const std::unordered_map<Speed, Direction, SpeedHasher> SPEED_TO_DIRECTION = {
        {{0, -1}, Direction::NORTH},
        {{0, 1}, Direction::SOUTH},
        {{-1, 0}, Direction::WEST},
        {{1, 0}, Direction::EAST},
        {{0, 0}, Direction::UNKNOWN}
    };

    const std::unordered_map<Direction, Direction> OPOSITE_DIRECTION = {
        {Direction::NORTH, Direction::SOUTH},
        {Direction::SOUTH, Direction::NORTH},
        {Direction::WEST, Direction::EAST},
        {Direction::EAST, Direction::WEST},
        {Direction::UNKNOWN, Direction::UNKNOWN}
    };



} //end namespace model

namespace savegame {

    struct SavedFileParameters {
        std::optional<std::string> save_file_path;
        std::optional<std::chrono::milliseconds> saved_tick_period;
    };

} //end namespace savegame