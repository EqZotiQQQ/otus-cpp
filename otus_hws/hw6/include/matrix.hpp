#pragma once

#include <tuple>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/composite_key.hpp>
#include <boost/multi_index/tag.hpp>

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

 
class MatrixBase {
public:
    MatrixBase(int default_value): default_value_(default_value) {}
    
    void insert(Cell&& cell) {
        matrix_.insert(std::move(cell));
    }

    template <typename ... Args>
    void insert(Args... args) {
        matrix_.emplace(args...);
    }
    
    void insert(int x, int y, int value) {
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

class MatrixImpl: public MatrixBase {
public:
    MatrixImpl(int default_value): MatrixBase(default_value) {}
private:
};
class MatrixProxy: public MatrixBase {
public:
    MatrixProxy(int default_value): MatrixBase(default_value) {}
private:
};
