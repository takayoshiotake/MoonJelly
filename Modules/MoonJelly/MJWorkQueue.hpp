//
//  MJWorkQueue.hpp
//  MoonJelly
//
//  Created by OTAKE Takayoshi on 2018/03/11.
//

#pragma once

#include <deque>
#include <thread>
#include <mutex>
#include <functional>
#include <future>
#include <utility>
#include <exception>

namespace MoonJelly {

    struct MJWorkQueueItem {
    private:
        bool is_cancelled_ = false;
    public:
        std::function<void(MJWorkQueueItem &) noexcept> work_;
        std::promise<void> promise_;
        
        MJWorkQueueItem(std::function<void(MJWorkQueueItem &)> work) noexcept
        : work_(std::move(work)) {
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
    };

    struct MJWorkQueue {
        bool shutdown_ = false;
        std::thread thread_;
        std::deque<MJWorkQueueItem> works_;
        std::mutex works_mutex_;
        std::condition_variable workable_;
        
        explicit MJWorkQueue() noexcept
        : thread_([this]() {
            while (!shutdown_) {
                MJWorkQueueItem item = [this]() -> MJWorkQueueItem {
                    std::unique_lock<std::mutex> lock(works_mutex_);
                    if (works_.empty()) {
                        workable_.wait(lock);
                    }
                    
                    auto item = std::move(works_.front());
                    works_.pop_front();
                    return item;
                }();
                
                item.work_(item);
            }
        }) {
        }
        
        ~MJWorkQueue() noexcept {
            {
                std::unique_lock<std::mutex> lock(works_mutex_);
                works_.clear();
            }
            async([this](MJWorkQueueItem const &) {
                shutdown_ = true;
            });
            thread_.join();
        }
        
        MJWorkQueueItem & async(std::function<void(MJWorkQueueItem const &) noexcept> work) noexcept {
            std::unique_lock<std::mutex> lock(works_mutex_);
            works_.push_back(MJWorkQueueItem([work = std::move(work)](MJWorkQueueItem & item) {
                if (item.is_cancelled()) {
                    item.promise_.set_exception(std::make_exception_ptr(std::runtime_error("cancelled")));
                    return;
                }
                work(item);
                item.promise_.set_value();
            }));
            auto & new_item = works_.back();
            workable_.notify_all();
            return new_item;
        }
        
        MJWorkQueue(MJWorkQueue const &) = delete;
        MJWorkQueue & operator=(MJWorkQueue const &) = delete;
    };

}
