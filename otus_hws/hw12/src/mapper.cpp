#include <cstdio>
#include <iostream>
#include <string>
#include <boost/program_options.hpp>
#include "spdlog/common.h"
#include <spdlog/spdlog.h>
#include "options.hpp"

static std::vector<std::string> split_csv_simple(const std::string& line) {
    std::vector<std::string> result;
    std::string cur;
    for (char c : line) {
        if (c == ',') {
            result.push_back(cur);
            cur.clear();
        } else {
            cur.push_back(c);
        }
    }
    result.push_back(cur);
    return result;
}

int main(int argc, char ** argv) {
    Options opts = parse_options(argc, argv);

    spdlog::set_level(spdlog::level::debug);

    // spdlog::debug("Options = {}", static_cast<int>(opts.mode)); no logs for this app

    std::string line;
    
    while (std::getline(std::cin, line)) {
        // spdlog::debug("{}", line);

        if (line.empty()) {
            continue;
        }

        auto cols = split_csv_simple(line);

        double price = 0.0;

        try {
            price = std::stod(cols[9]);
        } catch (...) {
            continue;
        }

        switch (opts.mode) {
        case Mode::MEAN:
            std::cout << "mean\t" << price << "\n";
            break;
        case Mode::VAR:
            std::cout << "var\t" << price << "\t" << (price * price) << "\t" << 1 << "\n";
            break;
        default:
            throw std::runtime_error("Bad mode");
        }
    }

    return 0;
}