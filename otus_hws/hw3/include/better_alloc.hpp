#pragma once

#include <cstddef>
#include <cstdlib>
#include <format>
#include <iostream>

template <typename T, size_t N>
class BetterAlloc {
public:
    using value_type = T;

    BetterAlloc() {
        init(N);
    };

    // required for map/unordered map. This constructor required to construct internal
    // types of stateless alloc
    template <typename U, size_t K>
    BetterAlloc(const BetterAlloc<U, K>&) noexcept {
    }

    template <typename U>
    struct rebind {
        using other = BetterAlloc<U, N>;
    };

    // n - amount of object to be placed in this memory blob
    T* allocate(size_t n) {
        size_t bytes_alloc_count = n * TYPE_SIZE;
        // if pool is full
        size_t bytes_left = N - pool_offset_b;
        if (pool_offset_b + bytes_alloc_count > bytes_left) {
            // std::cout << std::format("Use default alloc to allocate {} bytes ({} objects)\n",
            //                          n * TYPE_SIZE, n);
            return static_cast<T*>(std::malloc(bytes_alloc_count));
        }

        char* free_segmet = pool_blob + pool_offset_b;

        T* ptr = static_cast<T*>(static_cast<void*>(free_segmet));  // direct cast char*->T* not allowed

        // std::cout << std::format("Allocated {} bytes ({} objects) at {}\n", n * TYPE_SIZE, n,
        //                          pool_offset_b);

        pool_offset_b += bytes_alloc_count;

        return ptr;
    }

    void deallocate(T* ptr, [[maybe_unused]] size_t dealloc_bytes) noexcept {
        char* chptr = reinterpret_cast<char*>(ptr);
        bool is_pool_segment = (chptr >= pool_blob) && (chptr < (pool_blob + N));
        if (is_pool_segment) {
            // std::cout << std::format("Alloc not so smart to free mem: {} bytes\n",
            //                          dealloc_bytes * sizeof(T));
        } else {
            // std::cout << std::format("Free memory not from pool: {} bytes\n",
            //                          dealloc_bytes * sizeof(T));
            std::free(ptr);
        }
    }

    template <typename U, typename... Args>
    void construct(U* p, Args&&... args) {
        ::new ((void*) p) U(std::forward<Args>(args)...);
    }

    template <typename U>
    void destroy(U* p) {
        p->~U();
    }

    constexpr size_t max_size() const noexcept {
        return N / TYPE_SIZE;
    }

    size_t size() const noexcept {
        return N - pool_offset_b;
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

    char* pool_blob = nullptr;
    size_t pool_offset_b = 0;  // objects
    constexpr static size_t TYPE_SIZE = sizeof(T);
};