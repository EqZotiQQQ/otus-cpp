#include <boost/program_options.hpp>
#include <iostream>
#include <string>
#include <sstream>

#include "options.hpp"

int main(int argc, char* argv[]) {
    Options opts = parse_options(argc, argv);
    std::string line;

    double sum = 0.0;
    double sum_sq = 0.0;
    std::uint64_t count = 0;

    while (std::getline(std::cin, line)) {
        if (line.empty()) {
            continue;
        }

        std::istringstream iss(line);

        std::string key;
        iss >> key;

        switch (opts.mode) {
            case Mode::MEAN: {
                double price = 0.0;
                if (!(iss >> price)) {
                    continue;
                }

                sum += price;
                count += 1;
                break;
            }
            case Mode::VAR: {
                double x = 0.0;
                double x2 = 0.0;
                std::uint64_t one = 0;

                if (!(iss >> x >> x2 >> one)) {
                    continue;
                }

                sum += x;
                sum_sq += x2;
                count += one;
                break;
            }
            default:
                throw std::runtime_error("Bad mode");
        }
    }

    if (count == 0) {
        std::cerr << "No data to reduce\n";
        return 1;
    }

    switch (opts.mode) {
        case Mode::MEAN: {
            double mean = sum / static_cast<double>(count);
            std::cout << "mean\t" << mean << "\n";
            break;
        }
        case Mode::VAR: {
            double mean = sum / static_cast<double>(count);
            double variance = (sum_sq / static_cast<double>(count)) - mean * mean;
            std::cout << "var\t" << variance << "\n";
            break;
        }
    default:
        throw std::runtime_error("Bad mode");
    }

    return 0;
}
