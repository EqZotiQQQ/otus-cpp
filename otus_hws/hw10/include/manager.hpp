#pragma once

#include <mutex>
#include <memory>
#include <optional>
#include <thread>
#include <queue>
#include <condition_variable>
#include <sinks.hpp>


class Parser;

class Manager {
    using Block = std::vector<std::pair<std::string, int64_t>>;
public:
    static Manager& instance(std::optional<std::vector<std::shared_ptr<IBulkSink>>>&& sinks = {}) {
        static Manager inst(sinks.value());
        return inst;
    }

    Parser* create_parser(std::size_t bulk_size);

    void destroy_parser(Parser* p);

    void enqueue_log(Block block) {
        {
            std::lock_guard<std::mutex> lock(log_mtx_);
            log_queue_.push(std::move(block));
        }
        log_cv_.notify_one();
    }

    void enqueue_file(Block block) {
        {
            std::lock_guard<std::mutex> lock(file_mtx_);
            file_queue_.push(std::move(block));
        }
        file_cv_.notify_one();
    }
private:
    Manager(std::vector<std::shared_ptr<IBulkSink>> sinks): sinks_(sinks) {
        log_thread_ = std::thread(&Manager::log_worker, this);
        file_thread1_ = std::thread(&Manager::file_worker, this, 1);
        file_thread2_ = std::thread(&Manager::file_worker, this, 2);
    }
    
    ~Manager() {
        stop_ = true;

        log_cv_.notify_all();
        file_cv_.notify_all();

        if (log_thread_.joinable()) log_thread_.join();
        if (file_thread1_.joinable()) file_thread1_.join();
        if (file_thread2_.joinable()) file_thread2_.join();
    }


    void log_worker() {
        while (true) {
            Block block;
            {
                std::unique_lock<std::mutex> lock(log_mtx_);
                log_cv_.wait(lock, [&] {
                    return stop_ || !log_queue_.empty();
                });

                if (stop_ && log_queue_.empty())
                    return;

                block = std::move(log_queue_.front());
                log_queue_.pop();
            }

            if (!block.empty()) {
                std::vector<std::string> data;
                data.reserve(block.size());
                for (const auto& [command, stamp ]: block) {
                    data.push_back(command);
                }
                for (auto& sink : sinks_) {
                    if (sink->supports_log()) {
                        sink->flush(0, data, 0);
                    }
                }
            }
        }
    }

    void file_worker(int worker_id) {
        while (true) {
            Block block;
            {
                std::unique_lock<std::mutex> lock(file_mtx_);
                file_cv_.wait(lock, [&] {
                    return stop_ || !file_queue_.empty();
                });

                if (stop_ && file_queue_.empty())
                    return;

                block = std::move(file_queue_.front());
                file_queue_.pop();
            }

            if (!block.empty()) {
                std::vector<std::string> data;
                data.reserve(block.size());
                for (const auto& [command, stamp ]: block) {
                    data.push_back(command);
                }
                
                for (auto& sink : sinks_) {
                    if (sink->supports_file()) {
                        sink->flush(block.front().second, data, worker_id);
                    }
                }
            }
        }
    }

    Manager(const Manager&) = delete;
    Manager& operator=(const Manager&) = delete;

private:
    std::thread log_thread_;
    std::queue<Block> log_queue_;
    std::mutex log_mtx_;
    std::condition_variable log_cv_;

    std::thread file_thread1_;
    std::thread file_thread2_;
    std::queue<Block> file_queue_;
    std::mutex file_mtx_;
    std::condition_variable file_cv_;

    std::atomic<bool> stop_{false};

    std::vector<std::shared_ptr<IBulkSink>> sinks_;
};