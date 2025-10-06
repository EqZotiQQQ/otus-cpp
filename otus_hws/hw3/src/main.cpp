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

void map_hw_examples_custom() {
    std::map<int, int, std::less<int>, BetterAlloc<std::pair<const int, int>>>
        map_with_custom_allocator;
    for (size_t i = 0; i < 10; i++) {
        map_with_custom_allocator.emplace(std::make_pair(i, factorial(i)));
    }
    std::cout << std::format("Map with custom alloc:\n");
    for (const auto& [k, v] : map_with_custom_allocator) {
        std::cout << k << ": " << v << std::endl;
    }
    std::cout << std::endl;
}

void map_hw_examples_default() {
    std::map<int, int> map_with_default_allocator;
    for (size_t i = 0; i < 10; i++) {
        map_with_default_allocator.emplace(std::make_pair(i, factorial(i)));
    }
    std::cout << std::format("Map with default alloc:\n");
    for (const auto& [k, v] : map_with_default_allocator) {
        std::cout << k << ": " << v << std::endl;
    }
    std::cout << std::endl;
}

void custom_vec_examples() {
    AnotherVector<uint32_t, BetterAlloc<uint32_t>> v;

    for (int i = 0; i < 10; i++) {
        v.push_back(i);
    }

    std::cout << std::format("Vector with custom alloc:");
    for (const auto& item : v) {
        std::cout << " " << item;
    }
    std::cout << std::endl;
}

int main() {
    map_hw_examples_custom();
    map_hw_examples_default();
    custom_vec_examples();

    return 0;
}
