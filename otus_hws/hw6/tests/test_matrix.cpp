#define BOOST_TEST_MODULE test_version

#include <boost/test/unit_test.hpp>

#include "matrix.hpp"

BOOST_AUTO_TEST_SUITE(test_ips)

BOOST_AUTO_TEST_CASE(ma_cr) {
    MatrixProxy<int, 0> matrix;
    BOOST_CHECK(matrix.size() == 0);
    BOOST_CHECK(matrix[4][24] == 0);
}

BOOST_AUTO_TEST_CASE(ma_fill) {
    MatrixProxy<int, 0> matrix;
    for (int i = 0; i < 10; i++) {
        int reverse_value = 9 - i;
        matrix[i][i] = i;
        matrix[i][reverse_value] = reverse_value;
    }
    BOOST_CHECK(matrix.size() == 18);
}

BOOST_AUTO_TEST_CASE(ma_repl) {
    MatrixProxy<int, 0> matrix;
    BOOST_CHECK(matrix.size() == 0);
    matrix.insert(4, 2, 5);
    matrix.insert(4, 24, 53);
    BOOST_CHECK(matrix[4][24] == 53);
    BOOST_CHECK(matrix.size() == 2);

    matrix.insert(4, 24, 553);
    BOOST_CHECK(matrix.size() == 2);
    BOOST_CHECK(matrix[4][2] == 5);
    BOOST_CHECK(matrix[4][24] == 553);
}

BOOST_AUTO_TEST_SUITE_END()
