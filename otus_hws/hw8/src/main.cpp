#include "bayan.hpp"

int main(int argc, char** argv) {
    auto config = parse_cmd_line_arguments(argc, argv);

    if (config) {
        std::cout << fmt::format("{}\n", *config);
        auto files = collect_files(*config);
        auto dups = get_duplicated_files(files, *config);
        print_dups(dups);
    }

    return 0;
}
