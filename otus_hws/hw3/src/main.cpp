#include <algorithm>
#include <cassert>
#include <iostream>
#include <map>
#include <stdexcept>
#include <unordered_map>
#include <vector>

#include "alloc.hpp"
#include "better_alloc.hpp"
#include "not_even_vector.hpp"

size_t factorial(size_t value) noexcept {
    if (value <= 1) {
        return value;
    }
    return factorial(value - 1) * value;
}

void map_hw_examples() {
    std::cout << "Map examples:\n";
    std::map<int, int, std::less<int>, BetterAlloc<std::pair<const int, int>>>
        map_with_custom_allocator;
    for (size_t i = 0; i < 10; i++) {
        map_with_custom_allocator.emplace(std::make_pair(i, factorial(i)));
    }
    std::cout << std::format("Map with custom alloc: {}\n", map_with_custom_allocator);

    std::map<int, int> map_with_default_allocator;
    for (size_t i = 0; i < 10; i++) {
        map_with_default_allocator.emplace(std::make_pair(i, factorial(i)));
    }
    std::cout << std::format("Map with default alloc: {}\n", map_with_default_allocator);
}

void vec_examples() {
    std::cout << "Vec examples:\n";
    std::vector<uint32_t, BetterAlloc<uint32_t>> v;

    v.push_back(42);
    std::cout << std::format("Vector with custom alloc: {}\n", v);
    v.push_back(35);
    std::cout << std::format("Vector with custom alloc: {}\n", v);
    v.push_back(62);
    std::cout << std::format("Vector with custom alloc: {}\n", v);
    v.push_back(12);

    std::cout << std::format("Vector with custom alloc: {}\n", v);
}

void custom_vec_examples() {
    std::cout << "Custom vec examples:\n";
    AnotherVector<uint32_t, BetterAlloc<uint32_t>> v;

    v.push_back(42);
    v.push_back(35);
    v.push_back(62);
    v.push_back(12);

    std::cout << std::format("Vector with custom alloc: {}\n", v);
}

int main() {
    // vec_examples();
    // map_hw_examples();
    custom_vec_examples();

    // std::unordered_map<int, int, std::hash<int>, std::equal_to<int>,
    //                    Alloc<std::pair<const int, int>>>
    //     um;
    // um.reserve(500);
    // um.emplace(std::make_pair(42, 331));
    // um.emplace(std::make_pair(412, 333));
    // um.emplace(std::make_pair(425, 533));
    // um.emplace(std::make_pair(423, 313));

    // std::cout << std::format("{}\n", um);

    // std::map<int, int, std::less<int>, Alloc<std::pair<const int, int>>> m;
    // m.emplace(std::make_pair(42, 331));
    // m.emplace(std::make_pair(412, 333));
    // m.emplace(std::make_pair(425, 533));
    // m.emplace(std::make_pair(423, 313));

    // std::cout << std::format("{}\n", m);

    return 0;
}
