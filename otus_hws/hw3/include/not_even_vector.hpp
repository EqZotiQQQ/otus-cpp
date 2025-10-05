#pragma once

#include <concepts>
#include <format>
#include <iostream>
#include <memory>
#include <type_traits>

template <typename Alloc, typename T>
concept AllocatorFor = requires(Alloc a, T* p, size_t n, T&& value) {
    typename std::allocator_traits<Alloc>::value_type;
    requires std::same_as<typename std::allocator_traits<Alloc>::value_type, T>;

    { a.allocate(n) } -> std::convertible_to<T*>;
    a.deallocate(p, n);

    std::allocator_traits<Alloc>::construct(a, p, std::forward<T>(value));
    std::allocator_traits<Alloc>::destroy(a, p);
};

template <typename V, AllocatorFor<V> Alloc = std::allocator<V>>
class AnotherVector {
public:
    using value_type = V;
    using allocator_type = Alloc;
    using ptr = V*;
    using cptr = const V*;
    using ref = V&;
    using cref = const V&;

    AnotherVector() {
    }

    ~AnotherVector() {
        clear();
        if (data_) {
            std::allocator_traits<Alloc>::deallocate(alloc_, data_, capacity_);
            data_ = 0;
        }
    }

    AnotherVector(size_t initial_capacity) {
        reserve(initial_capacity);
    }

    AnotherVector(size_t initial_capacity, Alloc alloc) : alloc_(alloc) {
        reserve(initial_capacity);
    }

    void reserve(size_t new_capacity) {
        if (new_capacity > capacity_) {
            realloc(new_capacity);
        }
    }

    void shrink_to_fit() {
        if (capacity_ > size_) {
            realloc(size_);
        }
    }

    bool is_empty() const noexcept {
        return size_ == 0;
    }

    void pop_back() noexcept {
        if (is_empty()) {
            return;
        }
        std::allocator_traits<Alloc>::destroy(alloc_, std::addressof(data_[size_]));
        size_--;
    }

    void pop_front() noexcept {
        if (is_empty()) {
            return;
        }
        std::allocator_traits<Alloc>::destroy(alloc_, std::addressof(data_[0]));
        for (size_t i = 0; i < size_; i++) {
            std::allocator_traits<Alloc>::construct(alloc_, std::addressof(data_[i]),
                                                    std::move_if_noexcept(data_[i + 1]));
        }
        size_--;
    }

    void push_back(const value_type& new_obj) {
        if (size_ == capacity_) {
            auto new_cap = get_next_alloc_blob_size();
            realloc(new_cap);
        }
        // if alloc has construct method - he s gonna be invoked otherwice placement new
        std::allocator_traits<Alloc>::construct(alloc_, std::addressof(data_[size_]), new_obj);
        size_++;
    }

    void push_back(value_type&& new_obj) {
        if (size_ == capacity_) {
            auto new_cap = get_next_alloc_blob_size();
            realloc(new_cap);
        }
        // if alloc has construct method - he s gonna be invoked otherwice placement new
        std::allocator_traits<Alloc>::construct(alloc_, std::addressof(data_[size_]),
                                                std::move(new_obj));
        size_++;
    }

    void realloc(size_t new_cap) {
        ptr new_data = std::allocator_traits<allocator_type>::allocate(alloc_, new_cap);
        size_t i = 0;
        try {
            for (; i < size_; i++) {
                std::allocator_traits<Alloc>::construct(alloc_, std::addressof(new_data[i]),
                                                        std::move_if_noexcept(data_[i]));
            }
        } catch (...) {
            for (size_t j = 0; j < i; j++) {
                std::allocator_traits<Alloc>::destroy(alloc_, std::addressof(new_data[j]));
            }
            throw;
        }
        for (size_t j = 0; j < size_; j++) {
            std::allocator_traits<Alloc>::destroy(alloc_, std::addressof(data_[j]));
        }
        std::allocator_traits<Alloc>::deallocate(alloc_, data_, capacity_);

        data_ = new_data;
        capacity_ = new_cap;
    }

    void clear() noexcept {
        for (size_t j = 0; j < size_; j++) {
            std::allocator_traits<Alloc>::destroy(alloc_, std::addressof(data_[j]));
        }
        size_ = 0;
    }

    size_t get_next_alloc_blob_size() const noexcept {
        return capacity_ * 2;
    }

    size_t size() const noexcept {
        return size_;
    }

    size_t capacity() const noexcept {
        return capacity_;
    }

    const ptr data() const noexcept {
        return data_;
    }

    ref operator[](size_t index) noexcept {
        return data_[index];
    }

    cref operator[](size_t index) const noexcept {
        return data_[index];
    }

    ref at(size_t index) {
        if (index > size_) {
            throw std::out_of_range(
                std::format("Cannon access {} index of AnotherVector [0:{}]", index, size_));
        }

        return data_[index];
    }

    cref at(size_t index) const {
        if (index > size_) {
            throw std::out_of_range(
                std::format("Cannon access {} index of AnotherVector [0:{}]", index, size_));
        }

        return data_[index];
    }

private:
    Alloc alloc_{};
    size_t size_ = 0;
    size_t capacity_ = 0;
    ptr data_ = nullptr;
};

template <typename T, typename Alloc>
struct std::formatter<AnotherVector<T, Alloc>> : std::formatter<std::string> {
    template <typename FormatContext>
    auto format(const AnotherVector<T, Alloc>& vec, FormatContext& ctx) const {
        std::string out = "[";
        for (std::size_t i = 0; i < vec.size(); ++i) {
            out += std::format("{}", vec[i]);
            if (i + 1 < vec.size())
                out += ", ";
        }
        out += "]";
        return std::formatter<std::string>::format(out, ctx);
    }
};
