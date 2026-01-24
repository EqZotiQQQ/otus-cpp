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
    matrix.insert(4, 2, 5);
    matrix.insert(4, 24, 53);

    spdlog::info("Get cell: {} {} -> {}", 4, 2, matrix[4][2]);
    spdlog::info("Get cell: {} {} -> {}", 4, 1, matrix[4][1]);
    spdlog::info("Get cell: {} {} -> {}", 5, 5, matrix[5][5]);
    spdlog::info("Get cell: {} {} -> {}", 4, 24, matrix[4][24]);
    
    matrix.insert(4, 24, 553);
    spdlog::info("Get cell: {} {} -> {}", 4, 24, matrix[4][24]);

    return 0;
}
