#pragma once
#include "../other/tagged.h"

#include <string>
#include <unordered_map>
#include <vector>
#include <boost/json.hpp>
#include <utility>
#include <cstddef>



//Тут тоже всё переделываем на tag_invoke
namespace model {

    const std::string MAPS = "maps";
    const std::string MAP_ID = "id";
    const std::string MAP_NAME = "name";

    const std::string ROADS = "roads";
    const std::string ROAD_XO = "x0";
    const std::string ROAD_YO = "y0";
    const std::string ROAD_X1 = "x1";
    const std::string ROAD_Y1 = "y1";

    const std::string BUILDINGS = "buildings";
    const std::string BUILDING_X = "x";
    const std::string BUILDING_Y = "y";
    const std::string BUILDING_WIDTH = "w";
    const std::string BUILDING_HEIGHT = "h";

    const std::string OFFICES = "offices";
    const std::string OFFICE_ID = "id";
    const std::string OFFICE_X = "x";
    const std::string OFFICE_Y = "y";
    const std::string OFFICE_OFFSET_X = "offsetX";
    const std::string OFFICE_OFFSET_Y = "offsetY";

    const double DOG_SPEED_INIT = 1.0;

    using Dimension = int;
    using Coord = Dimension;
    namespace json = boost::json;

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

    class Road {
        struct HorizontalTag {
            explicit HorizontalTag() = default;
        };

        struct VerticalTag {
            explicit VerticalTag() = default;
        };

    public:
        constexpr static HorizontalTag HORIZONTAL{};
        constexpr static VerticalTag VERTICAL{};

        Road(HorizontalTag, Point start, Coord end_x) noexcept
            : start_{ start }
            , end_{ end_x, start.y } {
        }

        Road(VerticalTag, Point start, Coord end_y) noexcept
            : start_{ start }
            , end_{ start.x, end_y } {
        }

        bool IsHorizontal() const noexcept {
            return start_.y == end_.y;
        }

        bool IsVertical() const noexcept {
            return start_.x == end_.x;
        }

        Point GetStart() const noexcept {
            return start_;
        }

        Point GetEnd() const noexcept {
            return end_;
        }

    private:
        Point start_;
        Point end_;
    };

    void tag_invoke(json::value_from_tag, json::value& jv, const Road& road);
    Road tag_invoke(json::value_to_tag<Road>, const json::value& jv);

    class Building {
    public:
        explicit Building(Rectangle bounds) noexcept
            : bounds_{ bounds } {
        }

        const Rectangle& GetBounds() const noexcept {
            return bounds_;
        }

    private:
        Rectangle bounds_;
    };

    void tag_invoke(json::value_from_tag, json::value& jv, const Building& building);
    Building tag_invoke(json::value_to_tag<Building>, const json::value& jv);

    class Office {
    public:
        using Id = util::Tagged<std::string, Office>;

        Office(Id id, Point position, Offset offset) noexcept
            : id_{ std::move(id) }
            , position_{ position }
            , offset_{ offset } {
        }

        const Id& GetId() const noexcept {
            return id_;
        }

        Point GetPosition() const noexcept {
            return position_;
        }

        Offset GetOffset() const noexcept {
            return offset_;
        }

    private:
        Id id_;
        Point position_;
        Offset offset_;
    };

    void tag_invoke(json::value_from_tag, json::value& jv, const Office& office);
    Office tag_invoke(json::value_to_tag<Office>, const json::value& jv);

    class Map {
    public:
        using Id = util::Tagged<std::string, Map>;
        using Roads = std::vector<Road>;
        using Buildings = std::vector<Building>;
        using Offices = std::vector<Office>;

        Map(Id id, std::string name) noexcept
            : id_(std::move(id))
            , name_(std::move(name)) {
        }

        const Id& GetId() const noexcept {
            return id_;
        }

        const std::string& GetName() const noexcept {
            return name_;
        }

        const Buildings& GetBuildings() const noexcept {
            return buildings_;
        }

        const Roads& GetRoads() const noexcept {
            return roads_;
        }

        const Offices& GetOffices() const noexcept {
            return offices_;
        }

        void AddRoad(const Road& road) {
            roads_.emplace_back(road);
        }

        void AddRoads(Roads& roads) {
            for (const auto &item : roads) {
                AddRoad(item);
            }
        }

        void AddBuilding(const Building& building) {
            buildings_.emplace_back(building);
        }

        void AddBuildings(Buildings& buildings) {
            for (const auto& item : buildings) {
                AddBuilding(item);
            }
        }

        void AddOffice(Office office);

        void AddOffices(Offices& offices) {
            for (auto item : offices) {
                AddOffice(item);
            }
        }

        void SetDogSpeed(double speed) {
            dogSpeed_ = std::abs(speed);
        };

        double GetDogSpeed() const noexcept {
            return dogSpeed_;
        };

    private:
        using OfficeIdToIndex = std::unordered_map<Office::Id, size_t, util::TaggedHasher<Office::Id>>;

        Id id_;
        std::string name_;
        Roads roads_;
        Buildings buildings_;

        OfficeIdToIndex warehouse_id_to_index_;
        Offices offices_;

        double dogSpeed_{ DOG_SPEED_INIT };
    };

    void tag_invoke(json::value_from_tag, json::value& jv, const Map& map);
    Map tag_invoke(json::value_to_tag<Map>, const json::value& jv);

}  // namespace model