#pragma once

#include "generator.hpp"
#include "parser.hpp"
#include "sinks.hpp"

Generator<std::string> read_commands(std::istream& input);


void parse_stream_multithread(
    std::istream&,
    size_t,
    std::vector<std::shared_ptr<IBulkSink>>&&
) {
    // Parser parser(std::move(sinks));

    // parser.connect();

    // Generator<std::string> gen = read_commands(input);

    // while (gen.next()) {
    //     parser.consume(gen.value());
    // }

    // parser.finish();
}


Generator<std::string> read_commands(std::istream& input) {
    std::string line;
    while (std::getline(input, line)) {
        co_yield line;

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}
