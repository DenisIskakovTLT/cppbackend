#define _USE_MATH_DEFINES

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "../src/ModelLayer/collision/collision_detector.h"

using namespace std::literals;

// Напишите здесь тесты для функции collision_detector::FindGatherEvents
namespace collision_detector {

    /*Класс для тестов, с тестовой реализацией*/
    class ItemGathererProviderForTest : public ItemGathererProvider {
    public:
        virtual ~ItemGathererProviderForTest() = default;

        size_t ItemsCount() const {
            return items_.size();
        };

        Item GetItem(size_t idx) const override {           //Берем из базового класса
            return items_[idx];
        };

        size_t GatherersCount() const override {            //Берем из базового класса
            return gatherers_.size();
        };

        Gatherer GetGatherer(size_t idx) const override {   //Берем из базового класса
            return gatherers_[idx];
        };

        void AddItem(Item item) {
            items_.push_back(std::move(item));
        };

        void AddGatherer(Gatherer gatherer) {
            gatherers_.push_back(std::move(gatherer));
        };

    private:
        std::vector<Item> items_;
        std::vector<Gatherer> gatherers_;
    };


}

const std::string FIND_GATHER_EVENTS_TAG = "[FindGatherEvents]";

TEST_CASE("Собиратель нашёл предмет, двигаясь по оси Х", FIND_GATHER_EVENTS_TAG) {
    using Catch::Matchers::WithinAbs;
    collision_detector::Item item{ {5.0, 0}, 0.5 };                     //лут лежит на (5,0)
    collision_detector::Gatherer gatherer{ {0, 0}, {10.0, 0}, 0.7 };    //Искатель идёт от (0,0) до (10,0), искатель жирнее лута
    collision_detector::ItemGathererProviderForTest testProvider;       
    testProvider.AddItem(item);
    testProvider.AddGatherer(gatherer);
    std::vector<collision_detector::GatheringEvent> events = collision_detector::FindGatherEvents(testProvider);

    CHECK(events.size() == 1);          //Ивент должен быть один, потому что лут один

    /*Айдишники нулевые*/
    CHECK(events[0].gatherer_id == 0);
    CHECK(events[0].item_id == 0);

    CHECK_THAT(events[0].sq_distance, WithinAbs(0.0, 1e-10));
    CHECK_THAT(events[0].time, WithinAbs((item.position.x / gatherer.end_pos.x), 1e-10));
}

TEST_CASE("Собиратель нашёл предмет, двигаясь по оси У", FIND_GATHER_EVENTS_TAG) {
    using Catch::Matchers::WithinAbs;
    collision_detector::Item item{ {0, 5.0}, 0.5 };                     //лут лежит на (5,0)
    collision_detector::Gatherer gatherer{ {0, 0}, {0, 10.0}, 0.7 };    //Искатель идёт от (0,0) до (10,0), искатель жирнее лута
    collision_detector::ItemGathererProviderForTest testProvider;
    testProvider.AddItem(item);
    testProvider.AddGatherer(gatherer);
    std::vector<collision_detector::GatheringEvent> events = collision_detector::FindGatherEvents(testProvider);

    CHECK(events.size() == 1);          //Ивент должен быть один, потому что лут один

    /*Айдишники нулевые*/
    CHECK(events[0].gatherer_id == 0);
    CHECK(events[0].item_id == 0);

    CHECK_THAT(events[0].sq_distance, WithinAbs(0.0, 1e-10));
    CHECK_THAT(events[0].time, WithinAbs((item.position.y / gatherer.end_pos.y), 1e-10));
}

TEST_CASE("Собиратель нашёл предмет в конечной точке своего пути", FIND_GATHER_EVENTS_TAG) {
    using Catch::Matchers::WithinAbs;
    collision_detector::Item item{ {5.0, 0}, 0.5 };                     //лут лежит на (5,0)
    collision_detector::Gatherer gatherer{ {0, 0}, {5.0, 0}, 0.7 };    //Искатель идёт от (0,0) до (5,0), искатель жирнее лута
    collision_detector::ItemGathererProviderForTest testProvider;
    testProvider.AddItem(item);
    testProvider.AddGatherer(gatherer);
    std::vector<collision_detector::GatheringEvent> events = collision_detector::FindGatherEvents(testProvider);

    CHECK(events.size() == 1);          //Ивент должен быть один, потому что лут один

    /*Айдишники нулевые*/
    CHECK(events[0].gatherer_id == 0);
    CHECK(events[0].item_id == 0);

    CHECK_THAT(events[0].sq_distance, WithinAbs(0.0, 1e-10));
    CHECK_THAT(events[0].time, WithinAbs((item.position.x / gatherer.end_pos.x), 1e-10));
}

TEST_CASE("Собиратель нашёл предмет двигаясь в стороне от оси лута", FIND_GATHER_EVENTS_TAG) {
    using Catch::Matchers::WithinAbs;
    collision_detector::Item item{ {5.0, 0.5}, 0.0 };                     //лут лежит на (5,0.5)
    collision_detector::Gatherer gatherer{ {0, 0}, {5.0, 0.5}, 0.7 };    //Искатель идёт от (0,0) до (5,0.5)
    collision_detector::ItemGathererProviderForTest testProvider;
    testProvider.AddItem(item);
    testProvider.AddGatherer(gatherer);
    std::vector<collision_detector::GatheringEvent> events = collision_detector::FindGatherEvents(testProvider);

    CHECK(events.size() == 1);          //Ивент должен быть один, потому что лут один

    /*Айдишники нулевые*/
    CHECK(events[0].gatherer_id == 0);
    CHECK(events[0].item_id == 0);

    CHECK_THAT(events[0].sq_distance, WithinAbs(0.0, 1e-10));
    CHECK_THAT(events[0].time, WithinAbs((item.position.x / gatherer.end_pos.x), 1e-10));
}

TEST_CASE("Собиратель нашёл несколько предметов двигаяь по Х", FIND_GATHER_EVENTS_TAG) {
    using Catch::Matchers::WithinAbs;
    using Catch::Matchers::WithinRel;
    collision_detector::Item item1{ {5.0, 0.5}, 0.0 };                     //лут лежит на (5,0.5)
    collision_detector::Item item2{ {8.0, 0.5}, 0.0 };                     //лут лежит на (5,0.5)
    collision_detector::Gatherer gatherer{ {0, 0}, {10.0, 0.5}, 0.7 };    //Искатель идёт от (0,0) до (5,0.5)
    collision_detector::ItemGathererProviderForTest testProvider;
    testProvider.AddItem(item1);
    testProvider.AddItem(item2);
    testProvider.AddGatherer(gatherer);
    std::vector<collision_detector::GatheringEvent> events = collision_detector::FindGatherEvents(testProvider);

    CHECK(events.size() == 2);          //Дви ивента, потому что два лута

    /*Первый лут*/
    CHECK(events[0].gatherer_id == 0);
    CHECK(events[0].item_id == 0);
    CHECK_THAT(events[0].sq_distance, WithinAbs(0.0623441397, 1e-10));
    CHECK_THAT(events[0].time, WithinRel((item1.position.x / gatherer.end_pos.x), 1e-1));

    /*Второй лут*/
    CHECK(events[1].gatherer_id == 0);
    CHECK(events[1].item_id == 1);
    CHECK_THAT(events[1].sq_distance, WithinAbs(0.0099750623, 1e-10));
    CHECK_THAT(events[1].time, WithinRel((item2.position.x / gatherer.end_pos.x), 1e-1));
}


TEST_CASE("Собиратель нашёл один из двух предметов двигаяь по Х", FIND_GATHER_EVENTS_TAG) {
    using Catch::Matchers::WithinAbs;
    using Catch::Matchers::WithinRel;
    collision_detector::Item item1{ {5.0, 0.5}, 0.0 };                     //лут лежит на (5,0.5)
    collision_detector::Item item2{ {8.0, 0.5}, 0.0 };                     //лут лежит на (5,0.5)
    collision_detector::Gatherer gatherer{ {0, 0}, {7.0, 0.5}, 0.7 };       //Искатель идёт от (0,0) до (5,0.5)
    collision_detector::ItemGathererProviderForTest testProvider;
    testProvider.AddItem(item1);
    testProvider.AddItem(item2);
    testProvider.AddGatherer(gatherer);
    std::vector<collision_detector::GatheringEvent> events = collision_detector::FindGatherEvents(testProvider);

    CHECK(events.size() == 1);          

    CHECK(events[0].gatherer_id == 0);
    CHECK(events[0].item_id == 0);
    CHECK_THAT(events[0].sq_distance, WithinAbs(0.0203045685, 1e-10));
    CHECK_THAT(events[0].time, WithinRel((item1.position.x / gatherer.end_pos.x), 1e-1));


}

TEST_CASE("Два собирателя два лута", FIND_GATHER_EVENTS_TAG) {
    using Catch::Matchers::WithinAbs;
    collision_detector::Item item1{ {0.0, 5.0}, 0.0 };                     
    collision_detector::Item item2{ {8.0, 0.0}, 0.0 };                     
    collision_detector::Gatherer gatherer1{ {0, 0}, {0.0, 10.0}, 0.7 };    
    collision_detector::Gatherer gatherer2{ {0, 0}, {10.0, 0.0}, 0.7 };    
    collision_detector::ItemGathererProviderForTest testProvider;
    testProvider.AddItem(item1);
    testProvider.AddItem(item2);
    testProvider.AddGatherer(gatherer1);
    testProvider.AddGatherer(gatherer2);
    std::vector<collision_detector::GatheringEvent> events = collision_detector::FindGatherEvents(testProvider);

    CHECK(events.size() == 2);          //Дви ивента, потому что два лута

    /*Первый лут*/
    CHECK(events[0].gatherer_id == 0);
    CHECK(events[0].item_id == 0);
    CHECK_THAT(events[0].sq_distance, WithinAbs(0.0, 1e-10));
    CHECK_THAT(events[0].time, WithinAbs((item1.position.y / gatherer1.end_pos.y), 1e-10));

    /*Второй лут*/
    CHECK(events[1].gatherer_id == 1);
    CHECK(events[1].item_id == 1);
    CHECK_THAT(events[1].sq_distance, WithinAbs(0.0, 1e-10));
    CHECK_THAT(events[1].time, WithinAbs((item2.position.x / gatherer2.end_pos.x), 1e-10));
}

