//
//  MJWorkQueue2.hpp
//  MoonJelly
//
//  Created by OTAKE Takayoshi on 2018/03/16.
//

#pragma once

#include <memory>
#include <deque>
#include <chrono>
#include <thread>
#include <mutex>
#include <functional>
#include <future>
#include <utility>
#include <exception>

namespace MoonJelly {
    
    struct MJWorkQueue2Item {
        using time_point = std::chrono::system_clock::time_point;
    private:
        bool is_cancelled_ = false;
    public:
        std::function<void(MJWorkQueue2Item &)> work_;
        std::promise<void> promise_;
        time_point after_;
        
        explicit MJWorkQueue2Item(std::function<void(MJWorkQueue2Item &)> work, time_point after) noexcept
        : work_(std::move(work)), after_(after) {
        }
        
        bool is_cancelled() const noexcept {
            return is_cancelled_;
        }
        
        void cancel() noexcept {
            is_cancelled_ = true;
        }
        
        void wait() {
            if (is_cancelled_) {
                throw std::runtime_error("cancelled");
            }
            try {
                promise_.get_future().get();
            }
            catch (std::future_error const &) {
            }
        }
        
        void wait_for(double duration) {
            if (is_cancelled_) {
                throw std::runtime_error("cancelled");
            }
            if (promise_.get_future().wait_for(std::chrono::milliseconds(static_cast<long>(duration * 1000))) != std::future_status::ready) {
                throw std::runtime_error("timeout");
            }
            try {
                promise_.get_future().get();
            }
            catch (std::future_error const &) {
            }
        }
    };
    
    struct MJWorkQueue2 {
        bool shutdown_ = false;
        std::thread thread_;
        std::deque<std::shared_ptr<MJWorkQueue2Item>> works_;
        std::mutex works_mutex_;
        std::condition_variable workable_;
        
        explicit MJWorkQueue2() noexcept {
            thread_ = std::thread([this]() {
                while (!shutdown_) {
                    std::shared_ptr<MJWorkQueue2Item> item;
                    {
                        std::unique_lock<std::mutex> lock(works_mutex_);
                        // NOTE: Handle spurious wakeup
                        for (;;) {
                            if (works_.empty()) {
                                workable_.wait(lock);
                                if (works_.empty()) {
                                    continue;
                                }
                            }
                            auto t = works_.front()->after_;
                            if (t <= std::chrono::system_clock::now()) {
                                break;
                            }
                            workable_.wait_until(lock, t);
                        }
                        item = works_.front();
                        works_.pop_front();
                    }
                    item->work_(*item);
                }
            });
        }
        
        ~MJWorkQueue2() noexcept {
            {
                std::unique_lock<std::mutex> lock(works_mutex_);
                works_.clear();
            }
            async([&](auto &) {
                shutdown_ = true;
            });
            thread_.join();
        }
        
        MJWorkQueue2Item & async(std::function<void(MJWorkQueue2Item const &)> work) noexcept {
            return async(work, 0);
        }
        
        MJWorkQueue2Item & async(std::function<void(MJWorkQueue2Item const &)> work, double delay) noexcept {
            std::unique_lock<std::mutex> lock(works_mutex_);
            
            auto item = std::make_shared<MJWorkQueue2Item>([work = std::move(work)](MJWorkQueue2Item & item) {
                if (item.is_cancelled()) {
                    item.promise_.set_exception(std::make_exception_ptr(std::runtime_error("cancelled")));
                    return;
                }
                work(item);
                item.promise_.set_value();
            }, std::chrono::system_clock::now() + std::chrono::milliseconds(static_cast<long>(delay * 1000)));

            auto itr = std::find_if(works_.begin(), works_.end(), [&](auto x) -> bool {
                return x->after_ > item->after_;
            });
            works_.insert(itr, item);
            workable_.notify_all();
            return *item;
        }
        
        MJWorkQueue2(MJWorkQueue2 const &) = delete;
        MJWorkQueue2 & operator=(MJWorkQueue2 const &) = delete;
    };
    
}
