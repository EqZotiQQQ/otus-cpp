#include <algorithm>
#include <cmath>
#include <iostream>
#include <unordered_map>
#include <vector>

#include "hw3.hpp"
#include "hw4.hpp"

int p1820() {
    unsigned int n, k;
    scanf("%d%d", &n, &k);
    if (n <= k) {
        printf("%d", 2);
    } else {
        if ((n * 2) % k == 0) {
            printf("%d", (n * 2) / k);
        } else {
            printf("%d", (n * 2) / k + 1);
        }
    }
    return 0;
}

using u32 = unsigned int;
using i32 = int;

int p1197() {
    int tests_count;
    scanf("%d", &tests_count);
    // std::cout << tests_count  << std::endl;
    std::vector<std::pair<u32, u32>> base_pos;
    base_pos.reserve(tests_count);

    for (int i = 0; i < tests_count; i++) {
        char input[3];
        scanf("%2s", input);
        u32 ch_pos = input[0] - '0' - 49;
        u32 dig_pos = input[1] - '0' - 1;
        base_pos.emplace_back(ch_pos, dig_pos);
        // std::cout << "Input: " << input << std::endl;
        // std::cout << "also: " << ch_pos << " " << dig_pos << std::endl;
    }

    for (const auto& [x, y] : base_pos) {
        u32 possible_steps = 0;

        if (x >= 1 && y >= 2) {
            possible_steps++;
        }
        if (x >= 1 && y <= 5) {
            possible_steps++;
        }
        if (x >= 2 && y >= 1) {
            possible_steps++;
        }
        if (x >= 2 && y <= 6) {
            possible_steps++;
        }
        if (x <= 6 && y >= 2) {
            possible_steps++;
        }
        if (x <= 6 && y <= 5) {
            possible_steps++;
        }
        if (x <= 5 && y >= 1) {
            possible_steps++;
        }
        if (x <= 5 && y <= 6) {
            possible_steps++;
        }
        printf("%d\n", possible_steps);
    }

    return 0;
}

int p2066() {
    int x,y,z;
    scanf("%d%d%d", &x, &y, &z);
    int ret_v = 0;
    if (y > 1) {
        ret_v = x - (y * z);
    } else {
        ret_v = x - (y + z);
    }
    printf("%d\n", ret_v);
    return 0;
}

int p2100() {
    int guest_count;
    std::cin >> guest_count;
    int real_guests = 2;
    std::string guest;
    for (int i = 0; i < guest_count; i++) {
        std::cin >> guest;
        real_guests++;
        if (guest.find('+') != std::string::npos) {
            real_guests++;
        }
    }
    if (real_guests == 13) {
        real_guests++;
    }

    std::cout << real_guests * 100;
    return 0;
}

int p1880() {
    std::unordered_map<int, int> values;
    int count;
    for (int i = 0; i < 3; i++) {
        std::cin >> count;
        int numbers;
        for (int i = 0; i < count; i++) {
            std::cin >> numbers;
            values[numbers]++;
        }
    }
    int common_numbers = 0;
    for (const auto& [k, v]: values) {
        if (v == 3) {
            common_numbers++;
        }
    }
    std::cout << common_numbers << '\n';
    return 0;
}

int main() {
    // p1820();
    // p1197();
    // p2100();
    p1880();
    return 0;
}
