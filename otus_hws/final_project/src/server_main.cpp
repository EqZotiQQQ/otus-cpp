#include "server.hpp"
#include "server_options.hpp"

#include <spdlog/spdlog.h>

int main(int argc, char* argv[]) {
    Options options = parse_options(argc, argv);
    try {
        boost::asio::io_context io;

        Server server(io, options.port, options.history_depth_);

        spdlog::info("Chat server started on port {}", options.port);

        io.run();
    }
    catch (std::exception& e) {
        spdlog::error("Exception {}", e.what());
    }
}
