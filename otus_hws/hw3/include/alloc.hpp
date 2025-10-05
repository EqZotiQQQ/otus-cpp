#pragma once

#include <cstddef>
#include <cstdlib>
#include <format>
#include <iostream>

template <typename T>
class Alloc {
public:
    using value_type = T;

    Alloc() = default;

    // required for map/unordered map. This constructor required to construct internal
    // types of stateless alloc
    template <typename U>
    Alloc(const Alloc<U>&) noexcept {
    }

    // alloc_b - amount of object to be placed in this memory blob
    T* allocate(size_t alloc_b) {
        constexpr size_t max_alloc_size = std::numeric_limits<size_t>::max() / sizeof(T);
        if (alloc_b > max_alloc_size) {
            throw std::bad_alloc();
        }
        size_t alloc_bytes = alloc_b * sizeof(T);
        void* ptr = std::malloc(alloc_bytes);
        if (!ptr) {
            // std::format("Failed to allocate {} bytes", alloc_b).c_str()
            throw std::bad_alloc();
        }
        // T* object = new (ptr) T();

        std::cout << std::format("Allocated {} bytes\n", alloc_bytes);
        return static_cast<T*>(ptr);
    }

    void deallocate(T* ptr, [[maybe_unused]] size_t dealloc_bytes) noexcept {
        std::free(ptr);
        std::cout << std::format("Deallocated {} bytes\n", dealloc_bytes * sizeof(T));
    }

    ~Alloc() = default;

private:
};