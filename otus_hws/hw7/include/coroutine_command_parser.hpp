#pragma once

#include <coroutine>
#include <iomanip>
#include <iostream>
#include <optional>
#include <thread>
#include <vector>
#include <string>
#include <fstream>

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

struct CommandLineParser1 {
    void consume(std::string&& command) {
        command_decision(std::move(command));
    }

    void finish() {
        if (depth_ == 0) {
            flush_commands();
        } else {
            std::cout << "Faced not closed brackets. Ignoring output\n";
        }
        std::cout << "EOF faced\n";
    }

private:
    void command_decision(std::string&& command) {
        if (command.empty()) {
            flush_commands();
            std::cout << "Block ended\n";
        } else if (command == "{") {
            if (depth_ == 0) {
                flush_commands();
                std::cout << "Dynamic block started\n";
            }
            depth_++;
        } else if (command == "}") {
            if (depth_ == 1) {
                flush_commands();
                std::cout << "Dynamic block ended\n";
            }
            depth_--;
        } else {
            store_command(std::move(command));
        }
    }

    void flush_commands() {
        if (commands_.empty()) return;

        stdout_stored_commands();
        dump_stored_commands_to_files();
        commands_.clear();
    }

    void stdout_stored_commands() const {
        std::cout << "bulk: ";
        for (auto& c : commands_) std::cout << c << " ";
        std::cout << "\n";
    }

    void dump_stored_commands_to_files() const {
        auto now = std::chrono::system_clock::now();
        std::time_t t = std::chrono::system_clock::to_time_t(now);

        std::tm tm{};
        localtime_r(&t, &tm);

        std::ostringstream name;
        name << std::put_time(&tm, "%Y%m%d_%H%M%S") << ".txt";

        std::ofstream file(name.str());
        for (auto& c : commands_) {
            file << c << "\n";
        }
    }

    void store_command(std::string cmd) {
        commands_.push_back(std::move(cmd));
    }

private:
    int depth_ = 0;
    std::vector<std::string> commands_;
};

void parse_stream(std::istream& input) {
    CommandLineParser1 parser;
    Generator<std::string> gen = read_commands(input);

    while (gen.next()) {
        parser.consume(gen.value());
    }

    parser.finish();
}

