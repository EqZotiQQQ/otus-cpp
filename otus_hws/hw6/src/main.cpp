#include <algorithm>
#include <cassert>
#include <iostream>
#include <list>
#include <map>
#include <stdexcept>
#include <tuple>
#include <unordered_map>
#include <vector>

#include "matrix.hpp"

int main() {
    MatrixProxy matrix{-1};
    
    spdlog::info("Matrix size at the start: {}", matrix.size());

    assert(matrix.size() == 0);
    matrix.insert(4, 2, 5);
    matrix.insert(4, 24, 53);

    assert(matrix.size() == 2);
    spdlog::info("Matrix size after inserted two elements: {}", matrix.size());


    spdlog::info("Get cell: {} {} -> {}", 4, 2, matrix[4][2]);
    spdlog::info("Get cell: {} {} -> {}", 4, 1, matrix[4][1]);
    spdlog::info("Get cell: {} {} -> {}", 5, 5, matrix[5][5]);
    spdlog::info("Get cell: {} {} -> {}", 4, 24, matrix[4][24]);
    assert(matrix[4][24] == 53);
    assert(matrix[4][2] == 5);
    assert(matrix[4][4] == -1);
    assert(matrix[49][4] == -1);
    
    matrix.insert(4, 24, 553);
    spdlog::info("Get cell: {} {} -> {}", 4, 24, matrix[4][24]);

    spdlog::info("Matrix size at the veryend: {}", matrix.size());
    assert(matrix.size() == 2);
    return 0;
}
