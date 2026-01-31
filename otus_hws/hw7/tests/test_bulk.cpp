#define BOOST_TEST_MODULE test_version

#include <boost/test/unit_test.hpp>

#include "coroutine_bulk.hpp"

BOOST_AUTO_TEST_SUITE(test_bulk)


BOOST_AUTO_TEST_CASE(mo1) {
    { 
        auto sink = std::make_shared<CaptureSink>();
        std::vector<std::shared_ptr<IBulkSink>> sinks;
        sinks.push_back(sink);
        std::stringstream mock_text;
        mock_text << "cmd1\ncmd2\ncmd3\ncmd4\ncmd5\n";
        parse_stream(mock_text, 3, std::move(sinks));
        const auto buffer = sink->buffers();
        BOOST_CHECK(buffer.size() == 2);
        BOOST_CHECK(buffer[0].size() == 3);
        BOOST_CHECK(buffer[1].size() == 2);

        std::vector<std::vector<std::string>> expected = {{"cmd1", "cmd2", "cmd3"}, {"cmd4", "cmd5"}};
        
        BOOST_CHECK(buffer[0][0] == expected[0][0]);
        BOOST_CHECK(buffer[0][1] == expected[0][1]);
        BOOST_CHECK(buffer[0][2] == expected[0][2]);
        BOOST_CHECK(buffer[1][0] == expected[1][0]);
        BOOST_CHECK(buffer[1][1] == expected[1][1]);

    }
    {
        std::stringstream mock_text;
        auto sink = std::make_shared<CaptureSink>();
        std::vector<std::shared_ptr<IBulkSink>> sinks;
        sinks.push_back(sink);
        sinks.emplace_back(std::make_shared<CaptureSink>());
        mock_text << "cmd1\ncmd2\n{\ncmd3\ncmd4\n}\n{\ncmd5\ncmd6\n{\ncmd7\ncmd8\n}\ncmd9\n}\n{\ncmd10\ncmd11\n";
        parse_stream(mock_text, 3, std::move(sinks));
        const auto buffer = sink->buffers();
        BOOST_CHECK(buffer.size() == 3);
        BOOST_CHECK(buffer[0].size() == 2);
        BOOST_CHECK(buffer[1].size() == 2);
        BOOST_CHECK(buffer[2].size() == 5);
    }

}

BOOST_AUTO_TEST_SUITE_END()
