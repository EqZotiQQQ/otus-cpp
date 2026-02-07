#include <boost/asio.hpp>
#include <boost/program_options.hpp>
#include <spdlog/spdlog.h>

#include <iostream>
#include <memory>
#include <deque>
#include <array>
#include <thread>

using namespace boost;
namespace asio = boost::asio;
using tcp = asio::ip::tcp;
namespace po = boost::program_options;

struct Options {
    uint16_t port;
    boost::asio::ip::address_v4 ip_addr;
    std::string log_level;
};

Options parse_options(int argc, char* argv[]) {
    Options opts;
    std::string ip_as_str;

    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "this message")
        ("port,p", po::value<uint16_t>(&opts.port)->default_value(12345), "port")
        ("ip,i", po::value<std::string>(&ip_as_str)->default_value("127.0.0.1"), "ip address")
        ("log-level,l", po::value<std::string>(&opts.log_level)->default_value("info"),
         "log level: trace|debug|info|warn|error|critical|off");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << desc << std::endl;
        std::exit(0);
    }

    boost::system::error_code ec;
    opts.ip_addr = boost::asio::ip::make_address_v4(ip_as_str, ec);
    if (ec) {
        throw std::runtime_error(
            "Invalid IP address: " + ip_as_str +
            ". I expect something like 0-255.0-255.0-255.0-255");
    }

    return opts;
}

class Client : public std::enable_shared_from_this<Client> {
public:
    Client(asio::io_context& io_context,
           const boost::asio::ip::address_v4& host,
           unsigned short port)
        : socket_(io_context),
          resolver_(io_context),
          host_(host),
          port_(port) {}

    void start() {
        do_resolve();
    }

    void send(std::string msg) {
        asio::post(socket_.get_executor(),
            [self = shared_from_this(), msg = std::move(msg)]() mutable {
                bool write_in_progress = !self->write_queue_.empty();
                self->write_queue_.push_back(std::move(msg));
                if (!write_in_progress) {
                    self->do_write();
                }
            });
    }

private:
    void do_resolve() {
        auto self = shared_from_this();
        spdlog::debug("Resolving {}:{}", host_.to_string(), port_);

        resolver_.async_resolve(host_.to_string(), std::to_string(port_),
            [this, self](std::error_code ec, tcp::resolver::results_type endpoints) {
                if (!ec) {
                    spdlog::info("Resolved successfully");
                    do_connect(endpoints);
                } else {
                    spdlog::error("Resolve error: {}", ec.message());
                }
            });
    }

    void do_connect(const tcp::resolver::results_type& endpoints) {
        auto self = shared_from_this();
        spdlog::debug("Connecting...");

        asio::async_connect(socket_, endpoints,
            [this, self](std::error_code ec, const tcp::endpoint& ep) {
                if (!ec) {
                    spdlog::info("Connected to {}", ep.address().to_string());
                    do_read();
                } else {
                    spdlog::error("Connect error: {}", ec.message());
                }
            });
    }

    void do_write() {
        auto self = shared_from_this();
        asio::async_write(socket_, asio::buffer(write_queue_.front()),
            [this, self](std::error_code ec, std::size_t length) {
                if (!ec) {
                    spdlog::debug("Sent {} bytes", length);
                    write_queue_.pop_front();
                    if (!write_queue_.empty()) {
                        do_write();
                    }
                } else {
                    spdlog::error("Write error: {}", ec.message());
                }
            });
    }

    void do_read() {
        auto self = shared_from_this();
        socket_.async_read_some(asio::buffer(buffer_),
            [this, self](std::error_code ec, std::size_t length) {
                if (!ec) {
                    std::string msg(buffer_.data(), length);
                    spdlog::info("Received {} bytes: {}", length, msg);
                    do_read();
                } else {
                    spdlog::error("Read error: {}", ec.message());
                }
            });
    }

private:
    tcp::socket socket_;
    tcp::resolver resolver_;
    boost::asio::ip::address_v4 host_;
    unsigned short port_;

    std::array<char, 1024> buffer_;
    std::deque<std::string> write_queue_;
};

int main(int argc, char** argv) {
    try {
        auto options = parse_options(argc, argv);

        // Настраиваем уровень логирования
        spdlog::set_level(spdlog::level::from_str(options.log_level));
        spdlog::set_pattern("[%H:%M:%S.%e] [%^%l%$] %v");

        spdlog::info("Starting client. Target {}:{}",
                     options.ip_addr.to_string(), options.port);
        spdlog::info("Log level = {}", options.log_level);

        asio::io_context io_context;

        auto client = std::make_shared<Client>(io_context, options.ip_addr, options.port);
        client->start();

        std::thread io_thread([&]() {
            io_context.run();
        });

        std::string line;
        while (std::getline(std::cin, line)) {
            line.push_back('\n');
            spdlog::debug("Sending line: {}", line);
            client->send(line);
        }

        spdlog::info("EOF on stdin, stopping...");
        io_context.stop();
        io_thread.join();

    } catch (const std::exception& e) {
        spdlog::critical("Exception: {}", e.what());
        return 1;
    }

    return 0;
}
