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
    virtual ~MatrixInterface() = 0;
private:
};

class MatrixImpl: public MatrixInterface {
public:
    MatrixImpl(int default_value): default_value_(default_value) {}

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
private:
    Matrix matrix_;
    int default_value_;
};

class MatrixProxy: public MatrixInterface {
public:
    MatrixProxy(int default_value): matrix_impl_(default_value) {}

    void insert(int x, int y, int value) override {
        spdlog::info("Trying to add to {}:{} value {}", x, y, value);
        matrix_impl_.insert(x, y, value);
    }
private:
    MatrixImpl matrix_impl_;
};
