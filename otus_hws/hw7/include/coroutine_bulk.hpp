#pragma once

#include <chrono>
#include <coroutine>
#include <iomanip>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <thread>
#include <vector>
#include <string>
#include <fstream>
#include <memory>
#include <ranges>

namespace details {

int64_t now_ms() {
    return duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();
}

std::string trim_spaces(const std::string& s) {
    auto is_space = [](unsigned char c){ return std::isspace(c); };

    auto v = s | std::views::drop_while(is_space);
    auto rv = std::views::reverse(v) | std::views::drop_while(is_space) | std::views::reverse;

    return std::string(&*rv.begin(), std::ranges::distance(rv));
}

} // namespace details

template<typename T>
struct Generator {
    struct promise_type {
        T current_value;

        Generator get_return_object() {
            return Generator{
                std::coroutine_handle<promise_type>::from_promise(*this)
            };
        }

        std::suspend_always initial_suspend() { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }

        std::suspend_always yield_value(T value) {
            current_value = std::move(value);
            return {};
        }

        void return_void() {}
        void unhandled_exception() { std::terminate(); }
    };

    using handle_t = std::coroutine_handle<promise_type>;

    explicit Generator(handle_t h) : handle(h) {}
    Generator(const Generator&) = delete;
    Generator(Generator&& other) noexcept : handle(other.handle) {
        other.handle = nullptr;
    }

    ~Generator() {
        if (handle) handle.destroy();
    }

    bool next() {
        if (!handle || handle.done()) return false;
        handle.resume();
        return !handle.done();
    }

    T value() const {
        return handle.promise().current_value;
    }

private:
    handle_t handle;
};


Generator<std::string> read_commands(std::istream& input) {
    std::string line;
    while (std::getline(input, line)) {
        co_yield line;

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

class IBulkSink {
public:
    virtual ~IBulkSink() = default;
    virtual void flush(size_t stamp_ms, const std::vector<std::string>& commands) = 0;
};

class FileBulkSink: public IBulkSink {
public:
    void flush(size_t stamp_ms, const std::vector<std::string>& commands) override {
        std::ofstream file(std::to_string(stamp_ms) + ".txt");
        for (auto& c : commands) {
            file << c << "\n";
        }
    }
};

class ConsoleBulkSink: public IBulkSink {
public:
    void flush(size_t, const std::vector<std::string>& commands) override {
        std::cout << "bulk: ";
        for (auto& c : commands) std::cout << c << " ";
        std::cout << "\n";
    }
};

class CaptureSink : public IBulkSink {
public:
    void flush(size_t, const std::vector<std::string>& commands) override {
        std::vector<std::string> bulk;
        for (auto& c : commands) {
            bulk.push_back(c);
        }
        buffer_.push_back(std::move(bulk));
    }

    auto buffers() const {
        return buffer_;
    }
private:
    std::vector<std::vector<std::string>> buffer_;
};

class AsyncCommandParser {
public:
    AsyncCommandParser(size_t bulk_size, std::vector<std::shared_ptr<IBulkSink>>&& sinks): max_bulk_size_(bulk_size), sinks_(std::move(sinks)) {
        commands_.reserve(bulk_size);
    }

    void consume(std::string&& command) {
        command_decision(std::move(command));
    }

    void finish() {
        if (depth_ == 0) {
            flush_commands();
        }
    }

private:
    void command_decision(std::string&& command) {
        std::string trimmed_command = details::trim_spaces(command);

        if ((trimmed_command.contains("{") || trimmed_command.contains("}")) && trimmed_command.size() > 1) {
            throw std::runtime_error("I can't parse input with brackets and commands. Try split it to different input lines");
        }
        if (trimmed_command == "{") {
            if (depth_ == 0) {
                flush_commands();
            }
            depth_++;
        } else if (trimmed_command == "}") {
            if (depth_ == 0) {
                throw std::runtime_error("You can't type close bracket without typing opening bracket");
            }
            if (depth_ == 1) {
                flush_commands();
            }
            depth_--;
        } else {
            if (commands_.empty()) {
                first_bulk_command_stamp_ms_ = details::now_ms();
            }
            store_command(std::move(trimmed_command));
        }

        if ((commands_.size() >= max_bulk_size_) && depth_ < 1) {
            flush_commands();
        }
    }

    void flush_commands() {
        if (commands_.empty()) return;

        for (const auto& sink: sinks_) {
            sink->flush(first_bulk_command_stamp_ms_, commands_);
        }

        commands_.clear();
    }

    void store_command(std::string cmd) {
        commands_.push_back(std::move(cmd));
    }

private:
    size_t max_bulk_size_{};
    int64_t first_bulk_command_stamp_ms_{};
    int depth_ = 0;
    std::vector<std::string> commands_;
    std::vector<std::shared_ptr<IBulkSink>> sinks_;
};

void parse_stream(std::istream& input, size_t bulk_size, std::vector<std::shared_ptr<IBulkSink>>&& sinks) {
    AsyncCommandParser parser(bulk_size, std::move(sinks));

    Generator<std::string> gen = read_commands(input);

    while (gen.next()) {
        parser.consume(gen.value());
    }

    parser.finish();
}

