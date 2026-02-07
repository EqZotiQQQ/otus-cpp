#pragma once

#include "manager.hpp"
#include "utils.hpp"

#include <chrono>
#include <cstdint>
#include <vector>

class Parser {
public:
    explicit Parser(size_t bulk_size): max_bulk_size_(bulk_size) {}

    void receive(const char* data, std::size_t size) {
        auto commands = split(data, size);

        for (const auto& command: commands) {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            int64_t now = now_ms();
            command_decision(command, now);
        }
    }

    void flush() {
        if (!current_block_.empty()) {
            emit_block();
        }
    }


private:

    void command_decision(const std::string& command, int64_t rx_stamp_ms) {
        std::string trimmed_command = trim_spaces(command);

        if ((trimmed_command.contains("{") || trimmed_command.contains("}")) && trimmed_command.size() > 1) {
            throw std::runtime_error("I can't parse input with brackets and commands. Try split it to different input lines");
        }
        if (trimmed_command == "{") {
            if (depth_ == 0) {
                emit_block();
            }
            depth_++;
        } else if (trimmed_command == "}") {
            if (depth_ == 0) {
                throw std::runtime_error("You can't pass closed bracket without open bracket");
            }
            if (depth_ == 1) {
                emit_block();
            }
            depth_--;
        } else {
            if (current_block_.empty()) {
                first_bulk_command_stamp_ms_ = now_ms();
            }
            store_command(std::move(trimmed_command), rx_stamp_ms);
        }

        // std::cout << std::format("Added new message. Max bulk size: {}, current bulk: {}, depth: {}", max_bulk_size_, current_block_.size(), depth_) << std::endl;
        if ((current_block_.size() >= max_bulk_size_) && depth_ < 1) {
            emit_block();
        }
    }

    void store_command(std::string cmd, int64_t rx_stamp_ms) {
        current_block_.emplace_back(std::move(cmd), rx_stamp_ms);
    }

    int depth_ = 0;
    std::vector<std::pair<std::string, int64_t>> current_block_;
    int64_t first_bulk_command_stamp_ms_{};
    size_t max_bulk_size_{};

    void emit_block() {
        auto block = current_block_;

        Manager::instance().enqueue_log(current_block_);
        Manager::instance().enqueue_file(std::move(current_block_));

        current_block_.clear();
    }

};