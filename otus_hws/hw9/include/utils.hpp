#pragma once

#include <chrono>
#include <ranges>

static inline int64_t now_ms() {
    return duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();
}

static inline std::string trim_spaces(const std::string& s) {
    auto is_space = [](unsigned char c){ return std::isspace(c); };

    auto v = s | std::views::drop_while(is_space);
    auto rv = std::views::reverse(v) | std::views::drop_while(is_space) | std::views::reverse;

    return std::string(&*rv.begin(), std::ranges::distance(rv));
}

static inline std::vector<std::string> split(const char* data, size_t size) {
    std::vector<std::string> commands;
    int prev_interval = 0;
    for (size_t i = 0; i < size; i++) {
        if (data[i] == '\n') {
            commands.emplace_back(data + prev_interval, i - prev_interval + 1);
            prev_interval = i + 1;
        }
    }
    return commands;
}
