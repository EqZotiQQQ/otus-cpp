#include <cmath>
#include <iostream>
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

void impl(const std::vector<std::pair<u32, u32>>& base_pos) {
    for (const auto& [x, y] : base_pos) {
        u32 possible_steps = 2;
        if (x >= 1 && x <= 6) {
            possible_steps++;
        }
        if (y >= 1 && y <= 6) {
            possible_steps++;
        }
        if (x >= 2 && x <= 5) {
            possible_steps += 2;
        }
        if (y >= 2 && y <= 5) {
            possible_steps += 2;
        }

        std::cout << std::format("{}:{} = {}\n", x, y, possible_steps);
        // if (x == 0 && y == 0 || x == 0 && y == 7 || x == 7 && y == 7 || x == 7 && y == 0) {
        //     printf("2\n");
        // }
        // if (x == 0 && y > 0 || x == 0 && y < 7 || x == 7 && y < 7 || x < 7 && y == 0) {
        //     printf("3\n");
        // }
        // if (x >= 0 && y >= 2 && x <= 6 && y <= 6) {
        //     printf("8");
        // } else if () {

        // }
    }
}
// int p1197() {
//     constexpr static u32 bound = 7;
//     u32 tests_count;
//     scanf("%u", &tests_count);

//     std::vector<std::pair<u32, u32>> base_pos;
//     base_pos.reserve(tests_count);

//     for (u32 i = 0; i < tests_count; i++) {
//         char input[2];
//         scanf("%s", input);
//         u32 ch_pos = input[0] - '0' - 1;
//         u32 dig_pos = input[1] - '0' - 1;
//         std::cout << std::format("{}:{}\n", dig_pos, ch_pos);
//         base_pos.emplace_back(ch_pos, dig_pos);
//     }

//     for (const auto& [x, y] : base_pos) {

//         if (x >= 0 && y >= 2 && x <= 6 && y <= 6) {
//             printf("8");
//         } else if () {

//         }
//     }

//     return 0;
// }

int main() {
    // p1820();
    // p1197();
    std::vector<std::pair<u32, u32>> v{{0, 0}, {7, 7}, {0, 7}, {7, 0}};
    std::vector<std::pair<u32, u32>> v2{{1, 0}, {5, 7}, {1, 7}, {7, 1}};
    std::vector<std::pair<u32, u32>> v3{{4, 2}, {5, 3}, {3, 4}, {4, 4}};
    std::vector<std::pair<u32, u32>> v4{{2, 0}};

    impl(v);
    impl(v2);
    impl(v3);
    impl(v4);
    return 0;
}