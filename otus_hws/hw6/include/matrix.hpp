#pragma once

#include <tuple>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/composite_key.hpp>
#include <boost/multi_index/tag.hpp>

#include <spdlog/spdlog.h>

using namespace boost::multi_index;

struct Cell {
    int x;
    int y;
    int value;
};


struct by_x {};
struct by_y {};
struct by_xy {};

using Matrix = multi_index_container<
    Cell,
    indexed_by<
        ordered_unique<tag<by_xy>, composite_key<Cell, member<Cell, int, &Cell::x>, member<Cell, int, &Cell::y>>>,
        ordered_non_unique<tag<by_x>, member<Cell, int, &Cell::x>>,
        ordered_non_unique<tag<by_y>, member<Cell, int, &Cell::y>>
    >
>;

 
class MatrixInterface {
public:    
    virtual void insert(int x, int y, int value) = 0;
    virtual int get_value_at(int x, int y) const = 0;
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
    

class MatrixImpl: public MatrixInterface {
public:
    MatrixImpl(int default_value): default_value_(default_value) {
        spdlog::info("Created matrix with default value {}", default_value);
    }

    void insert(int x, int y, int value) override {
        auto& index = matrix_.get<by_xy>();
        
        auto it = index.find(std::make_tuple(x, y));
        
        if (value == default_value_) {
            if (it != index.end()) {
                index.erase(it);
            }
        } else {
            if (it != index.end()) {
                index.modify(it, [value](Cell& cell) {
                    cell.value = value;
                });
            } else {
                matrix_.emplace(x, y, value);
            }
        }
    }

    int get_value_at(int x, int y) const override {
        auto& index = matrix_.get<by_xy>();
        auto it = index.find(std::make_tuple(x, y));
        return it == index.end() ? default_value_ : it->value;
    }

private:
    Matrix matrix_;
    int default_value_;
};

class MatrixProxy: public MatrixInterface {
public:
    MatrixProxy(int default_value): matrix_impl_(default_value) {}

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
private:
    MatrixImpl matrix_impl_;
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
