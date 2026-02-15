#pragma once

#include "manager.hpp"
#include "utils.hpp"

#include <chrono>
#include <cstdint>
#include <vector>
#include <queue>
#include <memory>
#include <functional>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <future>

class AsyncParser : public std::enable_shared_from_this<AsyncParser> {
public:
    explicit AsyncParser(size_t bulk_size) 
        : max_bulk_size_(bulk_size)
        , stopped_(false)
        , depth_(0)
        , first_bulk_command_stamp_ms_(0)
        , worker_thread_([this] { process_queue(); }) {
    }

    ~AsyncParser() {
        stop();
    }

    // Асинхронный прием данных
    std::future<void> receive_async(const char* data, std::size_t size) {
        auto commands = split(data, size);
        
        auto promise = std::make_shared<std::promise<void>>();
        auto future = promise->get_future();
        
        {
            std::lock_guard<std::mutex> lock(queue_mutex_);
            for (const auto& command : commands) {
                command_queue_.push(Command{command, false});
            }
            // Добавляем специальный маркер для завершения обработки
            command_queue_.push(Command{"", true});
        }
        
        condition_.notify_one();
        
        // Сохраняем promise для уведомления о завершении
        std::lock_guard<std::mutex> lock(promises_mutex_);
        pending_promises_.push_back(std::move(promise));
        
        return future;
    }

    // Асинхронная принудительная отправка блока
    std::future<void> flush_async() {
        auto promise = std::make_shared<std::promise<void>>();
        auto future = promise->get_future();
        
        {
            std::lock_guard<std::mutex> lock(queue_mutex_);
            flush_requested_ = true;
            command_queue_.push(Command{"", true}); // Специальный маркер для flush
        }
        
        condition_.notify_one();
        
        std::lock_guard<std::mutex> lock(promises_mutex_);
        pending_promises_.push_back(std::move(promise));
        
        return future;
    }

    // Остановка парсера
    void stop() {
        stopped_ = true;
        {
            std::lock_guard<std::mutex> lock(queue_mutex_);
            command_queue_.push(Command{"", true}); // Завершающий маркер
        }
        condition_.notify_one();
        
        if (worker_thread_.joinable()) {
            worker_thread_.join();
        }
    }

private:
    struct Command {
        std::string text;
        bool is_special; // true для специальных команд (flush, stop)
    };

    void process_queue() {
        while (!stopped_) {
            std::vector<Command> batch;
            
            {
                std::unique_lock<std::mutex> lock(queue_mutex_);
                condition_.wait(lock, [this] { 
                    return !command_queue_.empty() || stopped_; 
                });
                
                if (stopped_ && command_queue_.empty()) {
                    break;
                }
                
                // Забираем все команды из очереди
                while (!command_queue_.empty()) {
                    batch.push_back(std::move(command_queue_.front()));
                    command_queue_.pop();
                }
            }
            
            // Обрабатываем команды
            for (const auto& cmd : batch) {
                process_command(cmd);
            }
            
            // Уведомляем о завершении обработки
            notify_completion();
        }
        
        // Финальный flush при остановке
        if (!current_block_.empty()) {
            emit_block_async();
        }
    }

    void process_command(const Command& cmd) {
        if (cmd.is_special) {
            if (flush_requested_) {
                flush_requested_ = false;
                handle_flush();
            }
            return;
        }
        
        // Имитация задержки обработки
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        int64_t now = now_ms();
        command_decision(cmd.text, now);
    }

    void handle_flush() {
        if (!current_block_.empty()) {
            emit_block_async();
        }
    }

    void command_decision(const std::string& command, int64_t rx_stamp_ms) {
        std::string trimmed_command = trim_spaces(command);

        if ((trimmed_command.contains("{") || trimmed_command.contains("}")) && trimmed_command.size() > 1) {
            throw std::runtime_error("I can't parse input with brackets and commands. Try split it to different input lines");
        }
        
        if (trimmed_command == "{") {
            if (depth_ == 0) {
                emit_block_async();
            }
            depth_++;
        } else if (trimmed_command == "}") {
            if (depth_ == 0) {
                throw std::runtime_error("You can't pass closed bracket without open bracket");
            }
            if (depth_ == 1) {
                emit_block_async();
            }
            depth_--;
        } else {
            if (current_block_.empty()) {
                first_bulk_command_stamp_ms_ = now_ms();
            }
            store_command(std::move(trimmed_command), rx_stamp_ms);
        }

        if ((current_block_.size() >= max_bulk_size_) && depth_ < 1) {
            emit_block_async();
        }
    }

    void store_command(std::string cmd, int64_t rx_stamp_ms) {
        current_block_.emplace_back(std::move(cmd), rx_stamp_ms);
    }

    void emit_block_async() {
        if (current_block_.empty()) {
            return;
        }
        
        // Асинхронная отправка блока в Manager
        auto block = std::make_shared<std::vector<std::pair<std::string, int64_t>>>(
            std::move(current_block_));
        current_block_.clear();
        
        // Используем пул потоков для асинхронной обработки
        std::thread([block]() {
            Manager::instance().enqueue_log(*block);
            Manager::instance().enqueue_file(std::move(*block));
        }).detach();
    }

    void notify_completion() {
        std::lock_guard<std::mutex> lock(promises_mutex_);
        for (auto& promise : pending_promises_) {
            promise->set_value();
        }
        pending_promises_.clear();
    }

    size_t max_bulk_size_;
    std::atomic<bool> stopped_;
    std::atomic<bool> flush_requested_{false};
    
    // Потокобезопасная очередь команд
    std::queue<Command> command_queue_;
    std::mutex queue_mutex_;
    std::condition_variable condition_;
    
    // Текущее состояние парсера (доступно только из рабочего потока)
    int depth_;
    std::vector<std::pair<std::string, int64_t>> current_block_;
    int64_t first_bulk_command_stamp_ms_;
    
    // Рабочий поток
    std::thread worker_thread_;
    
    // Promises для уведомления о завершении
    std::mutex promises_mutex_;
    std::vector<std::shared_ptr<std::promise<void>>> pending_promises_;
};

// Обновленная версия bulk_parser namespace
namespace bulk_parser {

void* connect(size_t block_size) {
    Manager& manager = Manager::instance(std::vector<std::shared_ptr<IBulkSink>>{
        std::make_shared<FileBulkSink>(),
        std::make_shared<ConsoleBulkSink>()
    });
    
    auto parser = std::make_shared<AsyncParser>(block_size);
    manager.register_parser(parser); // Новый метод для регистрации парсера
    
    // Возвращаем shared_ptr как void*, но нужно быть осторожным с управлением памятью
    auto* ptr = new std::shared_ptr<AsyncParser>(parser);
    return static_cast<void*>(ptr);
}

std::future<void> receive_async(void* context, const char* buffer, size_t size) {
    auto* parser_ptr = static_cast<std::shared_ptr<AsyncParser>*>(context);
    return (*parser_ptr)->receive_async(buffer, size);
}

std::future<void> flush_async(void* context) {
    auto* parser_ptr = static_cast<std::shared_ptr<AsyncParser>*>(context);
    return (*parser_ptr)->flush_async();
}

void disconnect(void* context) {
    auto* parser_ptr = static_cast<std::shared_ptr<AsyncParser>*>(context);
    
    // Асинхронно завершаем работу
    auto flush_future = (*parser_ptr)->flush_async();
    flush_future.wait(); // Ждем завершения flush
    
    (*parser_ptr)->stop();
    
    Manager::instance().unregister_parser(*parser_ptr);
    delete parser_ptr; // Освобождаем память
}

} // namespace bulk_parser