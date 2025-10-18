#define BOOST_TEST_MODULE test_version

#include <boost/test/unit_test.hpp>

#include "better_alloc.hpp"
#include "not_even_vector.hpp"

BOOST_AUTO_TEST_SUITE(test_vec)

BOOST_AUTO_TEST_CASE(test_push_back) {
    AnotherVector<int, BetterAlloc<int, 1024>> v;
    BOOST_CHECK(v.is_empty());
    BOOST_CHECK(v.capacity() == 0);

    v.push_back(42);
    v.push_back(55);
    v.push_back(77);
    v.push_back(11);

    BOOST_CHECK(v.size() == 4);
    BOOST_CHECK(v.capacity() == 4);

    v.push_back(12);
    BOOST_CHECK(v.size() == 5);
    BOOST_CHECK(v.capacity() == 8);

    BOOST_CHECK(v[0] == 42);
    BOOST_CHECK(v[1] == 55);
    BOOST_CHECK(v[2] == 77);
    BOOST_CHECK(v[3] == 11);
    BOOST_CHECK(v[4] == 12);

    v.pop_front();
    BOOST_CHECK(v.size() == 4);
    BOOST_CHECK(v[0] == 55);
    BOOST_CHECK(v[1] == 77);
    BOOST_CHECK(v[2] == 11);
    BOOST_CHECK(v[3] == 12);

    v.pop_back();
    BOOST_CHECK(v.size() == 3);
    BOOST_CHECK(v[0] == 55);
    BOOST_CHECK(v[1] == 77);
    BOOST_CHECK(v[2] == 11);

    v.shrink_to_fit();
    BOOST_CHECK(v.capacity() == 3);

    v.reserve(55);
    BOOST_CHECK(v.capacity() == 55);
    v.shrink_to_fit();
    BOOST_CHECK(v.capacity() == 3);
    BOOST_CHECK(v.at(1) == 77);

    BOOST_CHECK_THROW(v.at(44), std::out_of_range);

    v.clear();
    BOOST_CHECK(v.is_empty());
    BOOST_CHECK(v.capacity() == 3);
    v.shrink_to_fit();
    BOOST_CHECK(v.capacity() == 0);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(test_iter)

BOOST_AUTO_TEST_CASE(test_push_back) {
    AnotherVector<int, BetterAlloc<int, 1024>> v;
    BOOST_CHECK(v.is_empty());
    BOOST_CHECK(v.capacity() == 0);

    v.push_back(42);
    v.push_back(55);
    v.push_back(77);
    v.push_back(11);

    auto iter_begin = v.begin();
    BOOST_CHECK(*iter_begin == 42);
    BOOST_CHECK(*(++iter_begin) == 55);
    BOOST_CHECK(*(++iter_begin) == 77);
    BOOST_CHECK(*(++iter_begin) == 11);

    auto iter_end = v.end();
    iter_end--;
    BOOST_CHECK(*iter_end == 11);
    BOOST_CHECK(*(--iter_end) == 77);
    BOOST_CHECK(*(--iter_end) == 55);
    BOOST_CHECK(*(--iter_end) == 42);

    iter_begin = v.begin();
    *iter_begin = 66;
    BOOST_CHECK(v[0] == 66);

    iter_end = v.end();
    iter_end--;
    *iter_end = 33;
    BOOST_CHECK(v[3] == 33);

    BOOST_CHECK(v.end() - v.begin() == 4);
    BOOST_CHECK(v.begin() == v.begin());
    BOOST_CHECK(v.end() == v.end());
    BOOST_CHECK(v.end() > v.begin());

    std::size_t index = 0;
    for (const auto value : v) {
        BOOST_TEST(value == v[index]);
        ++index;
    }
}

BOOST_AUTO_TEST_SUITE_END()