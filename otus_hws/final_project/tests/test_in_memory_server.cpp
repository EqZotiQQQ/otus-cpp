#include "spdlog/spdlog.h"
#define BOOST_TEST_MODULE test_version

#include <boost/test/unit_test.hpp>

#include "in_memory_sql.hpp"

BOOST_AUTO_TEST_SUITE(test_bulk)


BOOST_AUTO_TEST_CASE(mo1) {
    auto expected_result = std::string("OK\n");
    spdlog::set_level(spdlog::level::debug);
    JoinStorage js;
    std::string ret = js.handle_command("INSERT A 0 lean\n");
    // spdlog::warn("ret={}", ret);
    BOOST_CHECK(ret == expected_result);

    ret = js.handle_command("INSERT A 1 sweater\n");
    // spdlog::warn("ret={}", ret);
    BOOST_CHECK(ret == expected_result);


    ret = js.handle_command("INSERT B 1 foo\n");
    // spdlog::warn("ret={}", ret);
    BOOST_CHECK(ret == expected_result);

    expected_result = "1,sweater,foo\nOK\n";
    ret = js.handle_command("INTERSECTION\n");
    // spdlog::warn("ret={}", ret);
    BOOST_CHECK(ret == expected_result);
}

BOOST_AUTO_TEST_CASE(base_fillin) {
    spdlog::set_level(spdlog::level::debug);
    JoinStorage js;
    
    BOOST_CHECK(js.handle_command("INSERT A 0 lean\n") == "OK\n");
    BOOST_CHECK(js.handle_command("INSERT A 1 sweater\n") == "OK\n");
    BOOST_CHECK(js.handle_command("INSERT A 2 frank\n") == "OK\n");
    BOOST_CHECK(js.handle_command("INSERT A 3 violation\n") == "OK\n");
    BOOST_CHECK(js.handle_command("INSERT A 4 quality\n") == "OK\n");
    BOOST_CHECK(js.handle_command("INSERT A 5 precision\n") == "OK\n");

    BOOST_CHECK(js.handle_command("INSERT B 3 proposal\n") == "OK\n");
    BOOST_CHECK(js.handle_command("INSERT B 4 example\n") == "OK\n");
    BOOST_CHECK(js.handle_command("INSERT B 5 lake\n") == "OK\n");
    BOOST_CHECK(js.handle_command("INSERT B 6 flour\n") == "OK\n");
    BOOST_CHECK(js.handle_command("INSERT B 7 wonder\n") == "OK\n");
    BOOST_CHECK(js.handle_command("INSERT B 8 selection\n") == "OK\n");

    auto expected = JoinResult{JoinRow{3, "violation", "proposal"},
    JoinRow{4,"quality","example"},
    JoinRow{5,"precision","lake"}
    };
    auto intersection = js.intersection();
    BOOST_CHECK_EQUAL_COLLECTIONS(intersection.cbegin(), intersection.cend(), expected.cbegin(), expected.cend());

    auto sym_diff = js.symmetric_difference();
    
    expected = JoinResult{
    JoinRow{0, "lean", ""},
    JoinRow{1,"sweater",""},
    JoinRow{2,"frank",""},
    JoinRow{6,"","flour"},
    JoinRow{7,"","wonder"},
    JoinRow{8,"","selection"}
    };
    BOOST_CHECK_EQUAL_COLLECTIONS(sym_diff.cbegin(), sym_diff.cend(), expected.cbegin(), expected.cend());


    js.handle_command("TRUNCATE A");
    auto a_table_content = js.get_table_content("A");
    BOOST_CHECK(a_table_content.empty());
}

BOOST_AUTO_TEST_SUITE_END()
