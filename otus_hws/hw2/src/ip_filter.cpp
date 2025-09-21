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
// ("",  '.') -> [""]
// ("11", '.') -> ["11"]
// ("..", '.') -> ["", "", ""]
// ("11.", '.') -> ["11", ""]
// (".11", '.') -> ["", "11"]
// ("11.22", '.') -> ["11", "22"]
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

        std::cout << std::format("{}: {}", collapsed_ip_, ip_) << std::endl;
    }

    std::string str() const noexcept {
        return std::format("{}: {}.{}.{}.{}", collapsed_ip_, ip_[0], ip_[1], ip_[2], ip_[3]);
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
            // std::cout << std::format("{}", Ip{v[0]}.str()) << std::endl;
        }

        // TODO reverse lexicographically sort

        std::cout << "\n\n\n\n";
        std::for_each(ip_pool.cbegin(), ip_pool.cend(),
                      [](const Ip &ip) { std::cout << ip.str() << std::endl; });

        std::cout << "\n\n\n\n";
        std::sort(ip_pool.begin(), ip_pool.end(),
                  [](const Ip &ip1, const Ip &ip2) { return ip1 < ip2; });

        std::for_each(ip_pool.cbegin(), ip_pool.cend(),
                      [](const Ip &ip) { std::cout << ip.str() << std::endl; });

        // for (std::vector<Ip>::const_iterator ip = ip_pool.cbegin();
        //      ip != ip_pool.cend(); ++ip) {
        //     for (std::vector<std::string>::const_iterator ip_part = ip->cbegin();
        //          ip_part != ip->cend(); ++ip_part) {
        //         if (ip_part != ip->cbegin()) {
        //             std::cout << ".";
        //         }
        //         std::cout << *ip_part;
        //     }
        //     std::cout << std::endl;
        // }

        // 222.173.235.246
        // 222.130.177.64
        // 222.82.198.61
        // ...
        // 1.70.44.170
        // 1.29.168.152
        // 1.1.234.8

        // TODO filter by first byte and output
        // ip = filter(1)

        // 1.231.69.33
        // 1.87.203.225
        // 1.70.44.170
        // 1.29.168.152
        // 1.1.234.8

        // TODO filter by first and second bytes and output
        // ip = filter(46, 70)

        // 46.70.225.39
        // 46.70.147.26
        // 46.70.113.73
        // 46.70.29.76

        // TODO filter by any byte and output
        // ip = filter_any(46)

        // 186.204.34.46
        // 186.46.222.194
        // 185.46.87.231
        // 185.46.86.132
        // 185.46.86.131
        // 185.46.86.131
        // 185.46.86.22
        // 185.46.85.204
        // 185.46.85.78
        // 68.46.218.208
        // 46.251.197.23
        // 46.223.254.56
        // 46.223.254.56
        // 46.182.19.219
        // 46.161.63.66
        // 46.161.61.51
        // 46.161.60.92
        // 46.161.60.35
        // 46.161.58.202
        // 46.161.56.241
        // 46.161.56.203
        // 46.161.56.174
        // 46.161.56.106
        // 46.161.56.106
        // 46.101.163.119
        // 46.101.127.145
        // 46.70.225.39
        // 46.70.147.26
        // 46.70.113.73
        // 46.70.29.76
        // 46.55.46.98
        // 46.49.43.85
        // 39.46.86.85
        // 5.189.203.46
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
