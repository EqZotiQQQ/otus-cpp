#include <array>
#include <cassert>
#include <cstdlib>
#include <format>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

std::vector<std::string> split(const std::string &str, char d) {
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

struct Ip {
    std::array<uint8_t, 4> ip_;
    std::uint32_t collapsed_ip_;

    Ip(const std::string &ip) {
        std::vector<std::string> v = split(ip, '.');
        if (v.size() != 4) {
            throw std::runtime_error(std::format("Bad IP: {}", ip));
        }
        for (int i = 0; i < 4; i++) {
            ip_[i] = static_cast<uint8_t>(std::stoi(v[i]));
        }

        collapsed_ip_ = (static_cast<uint32_t>(ip_[3])) | (static_cast<uint32_t>(ip_[2]) << 8) |
                        (static_cast<uint32_t>(ip_[1]) << 16) |
                        (static_cast<uint32_t>(ip_[0]) << 24);
    }

    std::string str() const noexcept {
        return std::format("{}.{}.{}.{}", ip_[0], ip_[1], ip_[2], ip_[3]);
    }

    bool operator>(const Ip &ip) const noexcept {
        return collapsed_ip_ > ip.collapsed_ip_;
    }

    bool operator<(const Ip &ip) const noexcept {
        return collapsed_ip_ < ip.collapsed_ip_;
    }

    bool operator==(const Ip &ip) const noexcept {
        return collapsed_ip_ == ip.collapsed_ip_;
    }

    Ip(const Ip &ip) {
        if (this == &ip) {
            return;
        }
        for (uint8_t i = 0; i < 4; i++) {
            ip_[i] = ip.ip_[i];
        }

        collapsed_ip_ = ip.collapsed_ip_;
    }

    Ip &operator=(const Ip &ip) {
        if (this == &ip) {
            return *this;
        }

        for (uint8_t i = 0; i < 4; i++) {
            ip_[i] = ip.ip_[i];
        }

        collapsed_ip_ = ip.collapsed_ip_;

        return *this;
    }
};

template <>
struct std::formatter<Ip> : std::formatter<std::string> {
    auto format(const Ip &ip, auto &ctx) {
        return std::formatter<std::string>::format(ip.str(), ctx);
    }
};

int main(int argc, char **argv) {
    if (argc != 2) {
        throw std::runtime_error("I expect csv with ips at second argument...");
    }

    try {
        std::ifstream file(argv[1]);

        if (!file.is_open()) {
            throw std::runtime_error(std::format("File {} not found!", argv[1]));
        }

        std::vector<Ip> ip_pool;

        for (std::string line; std::getline(file, line);) {
            std::vector<std::string> v = split(line, '\t');
            ip_pool.emplace_back(Ip{v[0]});
        }

        std::sort(ip_pool.begin(), ip_pool.end(),
                  [](const Ip &ip1, const Ip &ip2) { return ip1 > ip2; });

        std::for_each(ip_pool.cbegin(), ip_pool.cend(),
                      [](const Ip &ip) { std::cout << ip.str() << std::endl; });

        std::for_each(ip_pool.cbegin(), ip_pool.cend(), [](const Ip &ip) {
            if (ip.ip_[0] == 1) {
                std::cout << ip.str() << std::endl;
            }
        });

        std::for_each(ip_pool.cbegin(), ip_pool.cend(), [](const Ip &ip) {
            if ((ip.ip_[0] == 46) && (ip.ip_[1] == 70)) {
                std::cout << ip.str() << std::endl;
            }
        });

        std::for_each(ip_pool.cbegin(), ip_pool.cend(), [](const Ip &ip) {
            if ((ip.ip_[0] == 46) || (ip.ip_[1] == 46) || (ip.ip_[2] == 46) || (ip.ip_[3] == 46)) {
                std::cout << ip.str() << std::endl;
            }
        });

    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
