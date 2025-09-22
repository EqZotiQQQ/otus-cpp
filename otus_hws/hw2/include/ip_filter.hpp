#pragma once

#include <array>
#include <cstdint>
#include <string>
#include <vector>
#if __cplusplus >= 202302L
#include <format>
#else
#include <iomanip>
#include <sstream>
#include <utility>
#endif

struct Ip;
std::vector<std::string> split(const std::string& str, char d);
void lexicographically_sort(std::vector<Ip>& ip_pool);
std::vector<Ip> read_from_file(const std::string& file_path);
std::vector<Ip> read_from_cmd();
std::ostream& operator<<(std::ostream& os, const Ip& ip);

struct Ip {
    std::array<uint8_t, 4> ip_;
    uint32_t collapsed_ip_;

    Ip(const std::string& ip);
    Ip(const Ip& ip);
    Ip& operator=(const Ip& ip);
    std::string str() const noexcept;
    bool operator>(const Ip& ip) const noexcept;
    bool operator<(const Ip& ip) const noexcept;
    bool operator==(const Ip& ip) const noexcept;
    bool operator!=(const Ip& ip) const noexcept;
};

#if __cplusplus >= 202302L
template <>
struct std::formatter<Ip> : std::formatter<std::string> {
    auto format(const Ip& ip, auto& ctx) {
        return std::formatter<std::string>::format(ip.str(), ctx);
    }
};
#endif
