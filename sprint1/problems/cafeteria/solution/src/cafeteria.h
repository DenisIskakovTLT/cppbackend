#pragma once
#ifdef _WIN32
#include <sdkddkver.h>
#endif

#include <boost/asio/io_context.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/bind_executor.hpp>
#include <memory>

#include <chrono>

#include "hotdog.h"
#include "result.h"
#include "logger.h"

namespace net = boost::asio;

class ThreadChecker {
public:
    explicit ThreadChecker(std::atomic_int& counter)
        : counter_{ counter } {
    }

    ThreadChecker(const ThreadChecker&) = delete;
    ThreadChecker& operator=(const ThreadChecker&) = delete;

    ~ThreadChecker() {
        // assert ���������, ���� ����� ������� ������������ � �����������
        // �������� expected_counter_ ���������
        assert(expected_counter_ == counter_);
    }

private:
    std::atomic_int& counter_;
    int expected_counter_ = ++counter_;
};

// �������-���������� �������� ������������� ���-����
using HotDogHandler = std::function<void(Result<HotDog> hot_dog)>;

class Order : public std::enable_shared_from_this<Order> {
public:
    Order(net::io_context& io, int id, std::shared_ptr<GasCooker> gas_cooker, std::shared_ptr<Bread> bread, std::shared_ptr<Sausage> sausage, HotDogHandler handler)
        : io_{ io }
        , id_{ id }
        , gas_cooker_{ gas_cooker }
        , sausage_{ sausage }
        , bread_{ bread }
        , handler_{ std::move(handler) }
    {
    }

    //��� ���������� ���������� ���������� ������
    void Execute() {
        logger_.LogMessage("Start to prepare order #"s + getThreadIdStr());
        CreateBread();
        FrySausage();
    }

private:

    net::io_context& io_;
    int id_;
    HotDogHandler handler_;
    Logger logger_{ std::to_string(id_) };
    Timer bread_timer_{ io_ };
    Timer sausage_timer_{ io_ };

    std::atomic_int counter_{ 0 };

    std::shared_ptr<GasCooker> gas_cooker_;
    std::shared_ptr<Sausage> sausage_;
    std::shared_ptr<Bread> bread_;
    net::strand<net::io_context::executor_type> strand_{ net::make_strand(io_) };

    void CreateBread() {
        logger_.LogMessage("Making bread #" + std::to_string(bread_->GetId()) + " thread #"s + getThreadIdStr());

        
        bread_->StartBake(*gas_cooker_, [self = shared_from_this(), bread = bread_]() {  //���� ���� ������
            self->bread_timer_.expires_from_now(std::chrono::milliseconds(1001));
            self->logger_.LogMessage("Start bread_timer_ for bread #" + std::to_string(bread->GetId()) + " on thread #"s + getThreadIdStr());
            self->bread_timer_.async_wait(
                
                net::bind_executor(self->strand_, [self, bread](sys::error_code ec) {
                    bread->StopBaking();
                    self->OnBreadBaked(ec);
                    }));
        });
    }

    void FrySausage() {
        logger_.LogMessage("Fying sausage #" + std::to_string(sausage_->GetId()) + " thread #"s + getThreadIdStr());
        sausage_->StartFry(*gas_cooker_, [self = shared_from_this(), sausage = sausage_]() {
            self->sausage_timer_.expires_from_now(std::chrono::milliseconds(1501));
            self->logger_.LogMessage("Start sausage_timer_ for sausage #" + std::to_string(sausage->GetId()) + " thread #"s + getThreadIdStr());
            self->sausage_timer_.async_wait(
                // OnSausageFried ����� ������ ���������������� ������������ strand_
                net::bind_executor(self->strand_, [self, sausage](sys::error_code ec) {
                    sausage->StopFry();
                    self->OnSausageFried(ec);
                    }));
        });
    }

    void OnBreadBaked(sys::error_code ec) {
        ThreadChecker checker{ counter_ };
        if (ec) {
            logger_.LogMessage("Baking bread error : "s + ec.what());
        }
        else {
            logger_.LogMessage("Bread has been baked."sv);
        }
        CheckReadiness(ec);
    }

    void OnSausageFried(sys::error_code ec) {
        ThreadChecker checker{ counter_ };
        if (ec) {
            logger_.LogMessage("Frying sausage error: "s + ec.what());
        }
        else {
            logger_.LogMessage("Sausage has been fried."sv);
            // �������������, ��� ������� ���������� ????
            // onion_marinaded_ = true;
        }
        CheckReadiness(ec);
    }

    void CheckReadiness(sys::error_code ec) {
        // ���� ��� ���������� ������, ���������� ���-��� � ����������
        if (sausage_->IsCooked() && bread_->IsCooked()) {
            logger_.LogMessage("HoDog #"s + std::to_string(id_) + " has been cooked."s);
            logger_.LogMessage("Sausage #"s + std::to_string(sausage_->GetId()) + " cook duration is "s + std::to_string(duration_cast<milliseconds>(sausage_->GetCookDuration()).count()));
            logger_.LogMessage("Bread #"s + std::to_string(bread_->GetId()) + " cook duration is "s + std::to_string(duration_cast<milliseconds>(bread_->GetBakingDuration()).count()));
            handler_(HotDog(id_, sausage_, bread_));
        }
    }

};

// ����� "���������". ������� ���-����
class Cafeteria {
public:
    explicit Cafeteria(net::io_context& io)
        : io_{ io } {
    }

    // ���������� ������� ���-��� � �������� handler, ��� ������ ���-��� ����� �����.
    // ���� ����� ����� ���� ������ �� ������������� ������
    // using HotDogHandler = std::function<void(Result<HotDog> hot_dog)>;
    void OrderHotDog(HotDogHandler handler) {
        // �������� �������� �� ������
        auto bread = store_.GetBread();
        auto sausage = store_.GetSausage();

        std::make_shared<Order>(io_, next_order_id_++, gas_cooker_, bread, sausage, std::move(handler))->Execute();
    }

private:
    net::io_context& io_;
    // ������������ ��� �������� ������������ ���-����
    Store store_;
    // ������� �����. �� ������� ������ � ��������� ���� ������ ���� ������� ����� �� 8 �������
    // ����������� � ��� ������������� ������������ ���-����.
    // ����� �������� � ������� make_shared, ��� ��� GasCooker ����������� ��
    // enable_shared_from_this.
    std::shared_ptr<GasCooker> gas_cooker_ = std::make_shared<GasCooker>(io_);

    std::atomic_int next_order_id_ = 0;

};
