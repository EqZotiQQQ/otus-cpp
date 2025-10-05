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
    std::cout << "Custom map alloc example:\n";
    std::map<int, int, std::less<int>, BetterAlloc<std::pair<const int, int>>>
        map_with_custom_allocator;
    for (size_t i = 0; i < 10; i++) {
        map_with_custom_allocator.emplace(std::make_pair(i, factorial(i)));
    }
    std::cout << std::format("Map with custom alloc: {}\n", map_with_custom_allocator);
}

void map_hw_examples_default() {
    std::cout << "Default map example:\n";
    std::map<int, int> map_with_default_allocator;
    for (size_t i = 0; i < 10; i++) {
        map_with_default_allocator.emplace(std::make_pair(i, factorial(i)));
    }
    std::cout << std::format("Map with default alloc: {}\n", map_with_default_allocator);
}

void custom_vec_examples() {
    std::cout << "Custom vec with custom alloc:\n";
    AnotherVector<uint32_t, BetterAlloc<uint32_t>> v;

    for (int i = 0; i < 10; i++) {
        v.push_back(i);
    }

    std::cout << std::format("Vector with custom alloc: {}\n", v, v.size(), v.capacity());
}

int main() {
    map_hw_examples_custom();
    map_hw_examples_default();
    custom_vec_examples();

    return 0;
}
