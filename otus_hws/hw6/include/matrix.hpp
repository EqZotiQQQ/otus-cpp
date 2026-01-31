#pragma once

#include <tuple>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/composite_key.hpp>
#include <boost/multi_index/tag.hpp>

#include <spdlog/spdlog.h>

using namespace boost::multi_index;

template <typename T>
struct Cell {
    int x;
    int y;
    T value;
};


struct by_x {};
struct by_y {};
struct by_xy {};

template<typename T>
using Matrix = multi_index_container<
    Cell<T>,
    indexed_by<
        ordered_unique<tag<by_xy>, composite_key<Cell<T>, member<Cell<T>, int, &Cell<T>::x>, member<Cell<T>, int, &Cell<T>::y>>>,
        ordered_non_unique<tag<by_x>, member<Cell<T>, int, &Cell<T>::x>>,
        ordered_non_unique<tag<by_y>, member<Cell<T>, int, &Cell<T>::y>>
    >
>;

 
class MatrixInterface {
public:    
    virtual void insert(int x, int y, int value) = 0;
    virtual int get_value_at(int x, int y) const = 0;
    virtual size_t size() const = 0;
    virtual ~MatrixInterface() {};
private:
};

class CellProxy {
public:
    CellProxy(MatrixInterface& matrix, int x, int y)
        : matrix_(matrix), x_(x), y_(y) {}

    operator int() const {
        return matrix_.get_value_at(x_, y_);
    }

    CellProxy& operator=(int value) {
        matrix_.insert(x_, y_, value);
        return *this;
    }

private:
    MatrixInterface& matrix_;
    int x_;
    int y_;
};

class RowProxy {
public:
    RowProxy(MatrixInterface& matrix, int x)
        : matrix_(matrix), x_(x) {}

    CellProxy operator[](int y) {
        return CellProxy(matrix_, x_, y);
    }

private:
    MatrixInterface& matrix_;
    int x_;
};
    


template<typename T, int default_value>
class MatrixImpl: public MatrixInterface {
public:
    MatrixImpl() {
        spdlog::info("Created matrix with default value {}", default_value);
    }

    void insert(int x, int y, int value) override {
        auto& index = matrix_.template get<by_xy>();
        
        auto it = index.find(std::make_tuple(x, y));
        
        if (value == default_value) {
            if (it != index.end()) {
                index.erase(it);
            }
        } else {
            if (it != index.end()) {
                index.modify(it, [value](Cell<T>& cell) {
                    cell.value = value;
                });
            } else {
                matrix_.emplace(x, y, value);
            }
        }
    }

    int get_value_at(int x, int y) const override {
        auto& index = matrix_.template get<by_xy>();
        auto it = index.find(std::make_tuple(x, y));
        return it == index.end() ? default_value : it->value;
    }

    size_t size() const override {
        return matrix_.size();
    }

    using iterator = Matrix<T>::template index<by_xy>::type::iterator;
    using const_iterator = Matrix<T>::template index<by_xy>::type::const_iterator;

    iterator begin() {
        return matrix_.template get<by_xy>().begin();
    }

    iterator end() {
        return matrix_.template get<by_xy>().end();
    }

    const_iterator begin() const {
        return matrix_.template get<by_xy>().begin();
    }

    const_iterator end() const {
        return matrix_.template get<by_xy>().end();
    }

    const_iterator cbegin() const {
        return matrix_.template get<by_xy>().cbegin();
    }

    const_iterator cend() const {
        return matrix_.template get<by_xy>().cend();
    }


private:
    Matrix<T> matrix_;
};

template <typename T, int default_value>
class MatrixProxy: public MatrixInterface {
public:
    MatrixProxy() {}

    void insert(int x, int y, int value) override {
        spdlog::info("Trying to add to [{}:{}] value {}", x, y, value);
        matrix_impl_.insert(x, y, value);
    }

    int get_value_at(int x, int y) const override {
        return matrix_impl_.get_value_at(x, y);
    }

    RowProxy operator[](int x) {
        return RowProxy(*this, x);
    };

    size_t size() const override {
        return matrix_impl_.size();
    }
    using iterator = MatrixImpl<T, default_value>::iterator;
    using const_iterator = MatrixImpl<T, default_value>::const_iterator;

    iterator begin() {
        return matrix_impl_.begin();
    }

    iterator end() {
        return matrix_impl_.end();
    }

    const_iterator begin() const {
        return matrix_impl_.begin();
    }

    const_iterator end() const {
        return matrix_impl_.end();
    }

    const_iterator cbegin() const {
        return matrix_impl_.cbegin();
    }

    const_iterator cend() const {
        return matrix_impl_.cend();
    }

private:
    MatrixImpl<T, default_value> matrix_impl_;
};

template <>
struct fmt::formatter<CellProxy> : fmt::formatter<int> {
    template <typename FormatContext>
    auto format(const CellProxy& cell, FormatContext& ctx) const {
        return fmt::formatter<int>::format(
            static_cast<int>(cell), ctx
        );
    }
};
