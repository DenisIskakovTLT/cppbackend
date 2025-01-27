#pragma once

#include "geom.h"

#include <algorithm>
#include <optional>
#include <vector>

namespace collision_detector {

struct CollectionResult {
    bool IsCollected(double collect_radius) const {
        return proj_ratio >= 0 && proj_ratio <= 1 && sq_distance <= collect_radius * collect_radius;
    }

    // квадрат расстояния до точки
    double sq_distance;

    // доля пройденного отрезка
    double proj_ratio;
};

// Движемся из точки a в точку b и пытаемся подобрать точку c.
// Эта функция реализована в уроке.
CollectionResult TryCollectPoint(geom::Point2D a, geom::Point2D b, geom::Point2D c);


/*Структура заменена на класс, чтоб можно было наследовать*/
class Item {
public:
    Item(geom::Point2D position, double width)
        : position_(std::move(position))
        , width_(width)
        , type_(std::nullopt){
    }

    virtual const geom::Point2D& GetPosition() const {
        return position_;
    };

    virtual void SetPosition(geom::Point2D position) {
        position_ = std::move(position);
    };

    virtual const double GetWidth() const {
        return width_;
    };
    virtual void SetWidth(double width) {
        width_ = width;
    };

    virtual size_t GetType() const {
        return type_.value();
    };

    virtual void SetType(size_t type) {
        type_ = type;
    };

private:
    geom::Point2D position_;
    double width_;
    std::optional<size_t> type_;

};

struct Gatherer {
    geom::Point2D start_pos;
    geom::Point2D end_pos;
    double width;
};

class ItemGathererProvider {
protected:
    ~ItemGathererProvider() = default;

public:
    virtual size_t ItemsCount() const = 0;
    virtual Item GetItem(size_t idx) const = 0;
    virtual size_t GatherersCount() const = 0;
    virtual Gatherer GetGatherer(size_t idx) const = 0;
};

struct GatheringEvent {
    size_t item_id;
    size_t gatherer_id;
    double sq_distance;
    double time;
};

// Эту функцию вам нужно будет реализовать в соответствующем задании.
// При проверке ваших тестов она не нужна - функция будет линковаться снаружи.
std::vector<GatheringEvent> FindGatherEvents(const ItemGathererProvider& provider);

}  // namespace collision_detector