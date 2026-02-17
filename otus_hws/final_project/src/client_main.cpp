#include "client.hpp"
#include "client_options.hpp"


int main(int argc, char* argv[]) {
    Options options = parse_options(argc, argv);
    try {
        boost::asio::io_context io;

        tcp::resolver resolver(io);
        auto endpoints = resolver.resolve(options.ip_addr.to_string(), std::to_string(options.port));

        Client client(io, endpoints, options.user_name);

        std::thread t([&io]() { io.run(); });

        std::string line;
        while (std::getline(std::cin, line)) {
            client.write(line);
        }

        client.close();
        t.join();
    }
    catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }
}

