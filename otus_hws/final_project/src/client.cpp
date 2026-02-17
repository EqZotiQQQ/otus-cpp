#include "client.hpp"
#include <stdexcept>
#include <string>

namespace po = boost::program_options;


struct Options {
    uint16_t port;
    boost::asio::ip::address_v4 ip_addr;
    uint8_t log_level;
    std::string user_name;
};


Options parse_options(int argc, char* argv[]) {
    Options opts;

    std::string ip_as_str;

    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "this message")
        ("port,p", po::value<uint16_t>(&opts.port)->default_value(12345), "port")
        ("log-level,l", po::value<uint8_t>(&opts.log_level)->default_value(1), "0-info+, 1-warn+")
        ("user-name,u", po::value<std::string>(&opts.user_name), "displayed user name")
        ("ip,i", po::value<std::string>(&ip_as_str)->default_value("127.0.0.1"), "ip address");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << desc << std::endl;
        std::exit(0);
    }

    if (opts.user_name.empty()) {
        throw std::runtime_error("Unable to start without passing --username/-u");
    }
    boost::system::error_code ec;
    opts.ip_addr = boost::asio::ip::make_address_v4(ip_as_str, ec);
    if (ec) {
        throw std::runtime_error("Invalid IP address: " + ip_as_str + ". I expect something like 0-255.0-255.0-255.0-255");
    }

    return opts;
}


Client::Client(boost::asio::io_context& io, tcp::resolver::results_type endpoints, const std::string& user_name)
    : user_name_(user_name), io_(io), socket_(io) {
    do_connect(endpoints);
}

void Client::write(const std::string& msg) {
    boost::asio::post(io_, [this, msg]() {
        bool write_in_progress = !write_queue_.empty();
        write_queue_.push_back(msg + '\n');
        if (!write_in_progress) {
            do_write();
        }
    });
}

void Client::close() {
    boost::asio::post(io_, [this]() {
        socket_.close();
    });
}

void Client::do_connect(const tcp::resolver::results_type& endpoints) {
    boost::asio::async_connect(socket_, endpoints,
        [this](boost::system::error_code ec, tcp::endpoint) {
            if (!ec) {
                spdlog::info("Connection established");
                do_read();
                write(user_name_);
            } else {
                std::cerr << "Connect failed: " << ec.message() << "\n";
            }
        });
}

void Client::do_read() {
    boost::asio::async_read_until(socket_, buffer_, '\n',
        [this](boost::system::error_code ec, std::size_t /*len*/) {
            if (!ec) {
                std::istream is(&buffer_);
                std::string line;
                std::getline(is, line);
                spdlog::info("{}", line);
                do_read();
            } else {
                std::cerr << "Read error: " << ec.message() << "\n";
                socket_.close();
            }
        });
}

void Client::do_write() {
    boost::asio::async_write(socket_,
        boost::asio::buffer(write_queue_.front()),
        [this](boost::system::error_code ec, std::size_t /*len*/) {
            if (!ec) {
                write_queue_.pop_front();
                if (!write_queue_.empty()) {
                    do_write();
                }
            } else {
                std::cerr << "Write error: " << ec.message() << "\n";
                socket_.close();
            }
        });
}


int main(int argc, char* argv[]) {
    Options options = parse_options(argc, argv);
    try {
        boost::asio::io_context io;

        tcp::resolver resolver(io);
        auto endpoints = resolver.resolve(options.ip_addr.to_string(), std::to_string(options.port));

        Client client(io, endpoints, options.user_name);

        std::thread t([&io]() { io.run(); });

        // Read from stdin and send to server
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

