#include <algorithm>
#include <cassert>
#include <iostream>
#include <stdexcept>

#include "ip_filter.hpp"

int main(int argc, char** argv) {
    (void) argv;
    (void) argc;
    // if (argc != 2) {
    //     throw std::runtime_error("I expect csv with ips at second argument...");
    // }

    try {
        auto ip_pool = read_from_cmd();
        // auto ip_pool = read_from_file(argv[1]);

        lexicographically_sort(ip_pool);

        // print lex sorted
        std::for_each(ip_pool.cbegin(), ip_pool.cend(),
                      [](const Ip& ip) { std::cout << ip.str() << std::endl; });

        // [0]=1
        std::for_each(ip_pool.cbegin(), ip_pool.cend(), [](const Ip& ip) {
            if (ip.ip_[0] == 1) {
                std::cout << ip.str() << std::endl;
            }
        });

        // [0]=46 [1]=70
        std::for_each(ip_pool.cbegin(), ip_pool.cend(), [](const Ip& ip) {
            if ((ip.ip_[0] == 46) && (ip.ip_[1] == 70)) {
                std::cout << ip.str() << std::endl;
            }
        });

        // [any]=46
        std::for_each(ip_pool.cbegin(), ip_pool.cend(), [](const Ip& ip) {
            if (std::any_of(ip.ip_.cbegin(), ip.ip_.cend(),
                            [](const int& ip_part) { return ip_part == 46; })) {
                std::cout << ip.str() << std::endl;
            }
        });

    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
