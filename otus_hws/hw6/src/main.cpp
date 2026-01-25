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
    {    
        MatrixProxy<int, -1> matrix;
        
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

        spdlog::info("Matrix size at the very end: {}", matrix.size());
        assert(matrix.size() == 2);
    }
    {
        MatrixProxy<int, 0> matrix;
        for (int i = 0; i < 10; i++) {
            int reverse_value = 9 - i;
            matrix[i][i] = i;
            matrix[i][reverse_value] = reverse_value;
        }

        spdlog::info("Complete matrix:");
        for (int i = 1; i < 9; i++) {
            for (int j = 1; j < 9; j++) {
                std::cout << matrix[i][j] << " ";
            }
            std::cout << std::endl;
        }
        spdlog::info("Amount of filled elements: {}\n Filled elements:", matrix.size());

        assert(matrix.size() == 18);
        
        for (const auto& [x, y, v]: matrix) {
            spdlog::info("x={} y={} value={}", x, y, v);
        }
    }
    return 0;
}
