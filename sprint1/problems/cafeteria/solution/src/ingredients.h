#pragma once
#include <functional>
#include <optional>

#include "clock.h"
#include "gascooker.h"
#include "logger.h"

using namespace std::chrono;
using namespace std::literals;
/*
����� "�������".
��������� ���� ���������� �� ������� �����
*/
class Sausage : public std::enable_shared_from_this<Sausage> {
public:
    using Handler = std::function<void()>;

    explicit Sausage(int id)
        : id_{ id } {
    }

    int GetId() const {
        return id_;
    }

    // ���������� �������� �������������. �������� handler, ��� ������ �������� �������������
    void StartFry(GasCooker& cooker, Handler handler) {
        // ����� StartFry ����� ������� ������ ���� ���
        if (frying_start_time_) {
            throw std::logic_error("Frying already started");
        }

        // ��������� ��������� ����� StartFry
        frying_start_time_ = Clock::now();

        // ��������� ������ ������� �����
        gas_cooker_lock_ = GasCookerLock{ cooker.shared_from_this() };

        // �������� ������� ��� ������ �����������.
        // ����� �������� ����� �������� �������, ����������� shared_ptr � ������
        cooker.UseBurner([self = shared_from_this(), handler = std::move(handler)]{
            // ���������� ����� ������������ ������ �����������
            self->frying_start_time_ = Clock::now();
            handler();
            });
    }

    // ��������� ������������� � ����������� �������
    void StopFry() {
        if (!frying_start_time_) {
            throw std::logic_error("Frying has not started");
        }
        if (frying_end_time_) {
            throw std::logic_error("Frying has already stopped");
        }
        frying_end_time_ = Clock::now();
        // ����������� �������
        gas_cooker_lock_.Unlock();
    }

    bool IsCooked() const noexcept {
        return frying_start_time_.has_value() && frying_end_time_.has_value();
    }

    Clock::duration GetCookDuration() const {
        if (!frying_start_time_ || !frying_end_time_) {
            throw std::logic_error("Sausage has not been cooked");
        }
        return *frying_end_time_ - *frying_start_time_;
    }

private:
    int id_;
    GasCookerLock gas_cooker_lock_;
    std::optional<Clock::time_point> frying_start_time_;
    std::optional<Clock::time_point> frying_end_time_;
};

// ����� "����". ���� ���� ���������� ������ "�������"
class Bread : public std::enable_shared_from_this<Bread> {
public:
    using Handler = std::function<void()>;

    explicit Bread(int id)
        : id_{ id } {
    }

    int GetId() const {
        return id_;
    }

    // �������� ������������� ����� �� ������� �����. ��� ������ ������� ����� ������, �������
    // handler
    void StartBake(GasCooker& cooker, Handler handler) {
        logger_.LogMessage("StartBake on thread #"s + getThreadIdStr());

        if (baking_start_time_) {       //��� ��������� �� ��������� �����
            throw std::logic_error("Baking already started");
        }
 
        baking_start_time_ = Clock::now(); // ��� �� ��������� �����

        gas_cooker_mtx_ = GasCookerLock{ cooker.shared_from_this() };       //���-�� ���� ������� ������

        cooker.UseBurner([self = shared_from_this(), handler = std::move(handler)]{             //���������� �����, ���� ������ �� ���������� ������ �������
            
        self->baking_start_time_ = Clock::now();        //����� ������ ���������

        self->logger_.LogMessage("UseBurner on thread #"s + getThreadIdStr());//�Ĩ� ���� �� ���������� ���������
        handler();
            });
    }

    // ������������� ������������� ����� � ����������� �������.
    void StopBaking() {
        logger_.LogMessage("StopBaking on thread #"s + getThreadIdStr());
                
        if (!baking_start_time_) {                                  //��������� � ������ �� ������ ��������
            throw std::logic_error("Frying has not started");
        }
        
        if (baking_end_time_) {                                     //�������� �� ��������� �����
            throw std::logic_error("Frying has stopped");
        }
        
        baking_end_time_ = Clock::now();                            //���-�� ���� ���������� ��������� ������
        
        gas_cooker_mtx_.Unlock();                                   //����� �������
        logger_.LogMessage("Gas cooker free on thread #"s + getThreadIdStr());
    }

    // �����������, ������� �� ����
    bool IsCooked() const noexcept {
        return baking_start_time_.has_value() && baking_end_time_.has_value();              //���� ���� ����� ������ ��� ������ � ����� ���������, ������ �������
    }

    // ���������� ����������������� ��������� �����. ������� ����������, ���� ���� �� ��� �������
    Clock::duration GetBakingDuration() const {
        if (!baking_start_time_ || !baking_end_time_) {
            throw std::logic_error("Bread not baking");
        }
        return *baking_end_time_ - *baking_start_time_;
    }

private:
    int id_;
    Logger logger_{ "bread #" + std::to_string(id_) };
    
    GasCookerLock gas_cooker_mtx_;                 //���-�� ���� ������� ��� �����
    std::optional<Clock::time_point> baking_start_time_;
    std::optional<Clock::time_point> baking_end_time_;
};

// ����� ������������ (���������� ����������� � ���������� id)
class Store {
public:
    std::shared_ptr<Bread> GetBread() {
        return std::make_shared<Bread>(++next_id_);
    }

    std::shared_ptr<Sausage> GetSausage() {
        return std::make_shared<Sausage>(++next_id_);
    }

private:
    int next_id_ = 0;
};
