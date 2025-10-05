#pragma once

#include <cstddef>
#include <cstdlib>
#include <format>
#include <iostream>

template <typename T>
class BetterAlloc {
public:
    using value_type = T;

    BetterAlloc() = default;

    // required for map/unordered map. This constructor required to construct internal
    // types of stateless alloc
    template <typename U>
    BetterAlloc(const BetterAlloc<U>&) noexcept {
    }

    // n - amount of object to be placed in this memory blob
    T* allocate(size_t n) {
        if (!pool_blob) {
            init(POOL_SIZE_BYTES);
        }
        // if pool is full
        if (pool_offset_b + n * TYPE_SIZE > POOL_SIZE_BYTES - pool_offset_b) {
            // std::cout << std::format("Use default alloc to allocate {} bytes ({} objects)\n",
            //                          n * TYPE_SIZE, n);
            return static_cast<T*>(std::malloc(n * TYPE_SIZE));
        }

        char* free_segmet = pool_blob + pool_offset_b;

        T* ptr =
            static_cast<T*>(static_cast<void*>(free_segmet));  // direct cast char*->T* not allowed

        // std::cout << std::format("Allocated {} bytes ({} objects) at {}\n", n * TYPE_SIZE, n,
        //                          pool_offset_b);

        pool_offset_b += n * TYPE_SIZE;

        return ptr;
    }

    void deallocate(T* ptr, [[maybe_unused]] size_t dealloc_bytes) noexcept {
        char* chptr = reinterpret_cast<char*>(ptr);
        bool is_pool_segment = (chptr >= pool_blob) && (chptr < (pool_blob + POOL_SIZE_BYTES));
        if (is_pool_segment) {
            // std::cout << std::format("Alloc not so smart to free mem: {} bytes\n",
            //                          dealloc_bytes * sizeof(T));
        } else {
            // std::cout << std::format("Free memory not from pool: {} bytes\n",
            //                          dealloc_bytes * sizeof(T));
            std::free(ptr);
        }
    }

    size_t max_size() const noexcept {
        return POOL_SIZE_BYTES;
    }

    size_t size() const noexcept {
        return POOL_SIZE_BYTES - pool_offset_b;
    }

    ~BetterAlloc() {
        deinit();
    }

private:
    void init(size_t pool_size) {
        pool_blob = static_cast<char*>(std::malloc(pool_size));
        if (!pool_blob) {
            throw std::bad_alloc();
        }
        // std::cout << std::format("Created pool of {} bytes ({} objects)\n", pool_size,
        //                          static_cast<size_t>(pool_size / TYPE_SIZE));
        pool_offset_b = 0;
    }

    void deinit() {
        std::free(pool_blob);
        pool_blob = nullptr;
        pool_offset_b = 0;
    }

    static inline char* pool_blob = nullptr;
    static inline size_t pool_offset_b = 0;  // objects
    constexpr static size_t POOL_SIZE_BYTES = 1024;
    constexpr static size_t TYPE_SIZE = sizeof(T);
};