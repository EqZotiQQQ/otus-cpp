#define BOOST_TEST_MODULE test_version

#include <boost/test/unit_test.hpp>

#include "ip_filter.hpp"

BOOST_AUTO_TEST_SUITE(test_version)

BOOST_AUTO_TEST_CASE(mo) {
    Ip ip_less("44.33.11.22");
    Ip ip_greater("55.11.22.33");
    BOOST_CHECK(ip_less < ip_greater);
}

BOOST_AUTO_TEST_CASE(mo1) {
    std::vector<Ip> expected_order{
        {"40.50.22.33"}, {"40.30.11.22"}, {"40.10.22.33"}};  // 51 > 33 > 11
    std::vector<Ip> ips{{"40.30.11.22"}, {"40.10.22.33"}, {"40.50.22.33"}};
    lexicographically_sort(ips);
    BOOST_CHECK_EQUAL_COLLECTIONS(ips.begin(), ips.end(), expected_order.begin(),
                                  expected_order.end());
}

BOOST_AUTO_TEST_SUITE_END()
