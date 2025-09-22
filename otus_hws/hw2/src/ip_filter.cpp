#include <array>
#include <cassert>
#if __cplusplus >= 202302L
#include <format>
#else
#include <cstdlib>
#include <iomanip>
#include <sstream>
#include <utility>
#endif
#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "ip_filter.hpp"

std::vector<std::string> split(const std::string& str, char d) {
    std::vector<std::string> r;

    std::string::size_type start = 0;
    std::string::size_type stop = str.find_first_of(d);
    while (stop != std::string::npos) {
        r.push_back(str.substr(start, stop - start));

        start = stop + 1;
        stop = str.find_first_of(d, start);
    }

    r.push_back(str.substr(start));

    return r;
}

void lexicographically_sort(std::vector<Ip>& ip_pool) {
    std::sort(ip_pool.begin(), ip_pool.end(),
              [](const Ip& ip1, const Ip& ip2) { return ip1 > ip2; });
}

std::vector<Ip> read_from_cmd() {
    std::vector<Ip> ip_pool;

    for (std::string line; std::getline(std::cin, line);) {
        std::vector<std::string> v = split(line, '\t');
        ip_pool.emplace_back(Ip{v[0]});
    }

    return ip_pool;
}

std::vector<Ip> read_from_file(const std::string& file_path) {
    std::ifstream file(file_path);

    if (!file.is_open()) {
#if __cplusplus >= 202302L
        throw std::runtime_error(std::format("File {} not found!", file_path));
#else
        throw std::runtime_error("File {} not found!" + file_path);
#endif
    }

    std::vector<Ip> ip_pool;

    for (std::string line; std::getline(file, line);) {
        std::vector<std::string> v = split(line, '\t');
        ip_pool.emplace_back(Ip{v[0]});
    }

    return ip_pool;
}

Ip::Ip(const std::string& ip) {
    std::vector<std::string> v = split(ip, '.');
    if (v.size() != 4) {
#if __cplusplus >= 202302L
        throw std::runtime_error(std::format("Bad string format: {}", ip));
#else
        throw std::runtime_error("Bad IP: " + ip);
#endif
    }
    for (int i = 0; i < 4; i++) {
        ip_[i] = static_cast<uint8_t>(std::stoi(v[i]));
    }

    collapsed_ip_ = (static_cast<uint32_t>(ip_[3])) | (static_cast<uint32_t>(ip_[2]) << 8) |
                    (static_cast<uint32_t>(ip_[1]) << 16) | (static_cast<uint32_t>(ip_[0]) << 24);
}

Ip::Ip(const Ip& ip) {
    if (this == &ip) {
        return;
    }
    for (uint8_t i = 0; i < 4; i++) {
        ip_[i] = ip.ip_[i];
    }

    collapsed_ip_ = ip.collapsed_ip_;
}

Ip& Ip::operator=(const Ip& ip) {
    if (this == &ip) {
        return *this;
    }

    for (uint8_t i = 0; i < 4; i++) {
        ip_[i] = ip.ip_[i];
    }

    collapsed_ip_ = ip.collapsed_ip_;

    return *this;
}

bool Ip::operator>(const Ip& ip) const noexcept {
    return collapsed_ip_ > ip.collapsed_ip_;
}

bool Ip::operator<(const Ip& ip) const noexcept {
    return collapsed_ip_ < ip.collapsed_ip_;
}

bool Ip::operator==(const Ip& ip) const noexcept {
    return collapsed_ip_ == ip.collapsed_ip_;
}

bool Ip::operator!=(const Ip& ip) const noexcept {
    return !(collapsed_ip_ == ip.collapsed_ip_);
}

std::string Ip::str() const noexcept {
#if __cplusplus >= 202302L
    return std::format("{}.{}.{}.{}", ip_[0], ip_[1], ip_[2], ip_[3]);
#else
    std::ostringstream ostream;
    ostream << static_cast<uint32_t>(ip_[0]) << '.' << static_cast<uint32_t>(ip_[1]) << '.'
            << static_cast<uint32_t>(ip_[2]) << '.' << static_cast<uint32_t>(ip_[3]);
    return ostream.str();
#endif
}

std::ostream& operator<<(std::ostream& os, const Ip& ip) {
    os << ip.str();
    return os;
}
