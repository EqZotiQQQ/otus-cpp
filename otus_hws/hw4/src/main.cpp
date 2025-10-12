#include <algorithm>
#include <cassert>
#include <iostream>
#include <list>
#include <map>
#include <stdexcept>
#include <unordered_map>
#include <vector>

#include "ip_printer.hpp"

int main() {
    print_ip(int16_t{256});                          // 1.0
    print_ip(int8_t{-1});                            // 255
    print_ip(int8_t{15});                            // 15
    print_ip(int16_t{0});                            // 0.0
    print_ip(int16_t{3221});                         // 0.0
    print_ip(int32_t{2130706433});                   // 127.0.0.1
    print_ip(int32_t{2130706435});                   // 127.0.0.3
    print_ip(int64_t{8875824491850138409});          // 123.45.67.89.101.112.131.41
    print_ip(std::string{"Hello, World !"});         // Hello, World!      // Hello, World!
    print_ip(std::vector<int>{100, 200, 300, 400});  // 100.200.300.400
    print_ip(std::list<short>{400, 300, 200, 100});  // 400.300.200.100

    print_ip(std::make_tuple(123, 456, 789, 0));  // 123.456.789.0
    // use_decltype::print_ip(std::make_tuple(123, 456, 789, "zzz"));  // 123.456.789.0

    return 0;
}
