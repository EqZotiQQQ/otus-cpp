#include <boost/asio/ip/address_v4.hpp>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/asio/error.hpp>
#include <boost/program_options.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <memory>
#include <array>
#include "in_memory_sql.hpp"
#include "spdlog/common.h"
#include <spdlog/spdlog.h>

using namespace boost;
using boost::asio::ip::tcp;
namespace po = boost::program_options;

namespace server {

struct Options {
    uint16_t port;
    boost::asio::ip::address_v4 ip_addr;
    unsigned log_level;
};

Options parse_options(int argc, char* argv[]) {
    Options opts;

    std::string ip_as_str;

    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "this message")
        ("port,p", po::value<uint16_t>(&opts.port)->default_value(12345), "port")
        ("log-level,l", po::value<unsigned>(&opts.log_level)->default_value(1), "0-info+, 1-warn+")
        ("ip,i", po::value<std::string>(&ip_as_str)->default_value("127.0.0.1"), "ip address");

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
        throw std::runtime_error("Invalid IP address: " + ip_as_str + ". I expect something like 0-255.0-255.0-255.0-255");
    }

    return opts;
}

class Session : public std::enable_shared_from_this<Session> {
public:
    explicit Session(tcp::socket socket, const std::shared_ptr<JoinStorage>& storage)
        : socket_(std::move(socket)),
          id_(boost::uuids::random_generator()()),
          storage_(storage) {
        spdlog::info("Created session {}", boost::uuids::to_string(id_));
    }

    void start() {
        do_read();
    }

    ~Session() {
        spdlog::info("Session destroyed {}", boost::uuids::to_string(id_));
    }

    boost::uuids::uuid id() const { return id_; }
private:
    void do_read() {
        auto self = shared_from_this();
        socket_.async_read_some(asio::buffer(buffer_),
            [this, self](boost::system::error_code ec, std::size_t length) {
                if (!ec) {
                    std::string msg(buffer_.data(), length);
                    spdlog::info("Session {} received {} bytes with data:  {}", boost::uuids::to_string(id_), length, msg);
                    std::string command_result = storage_->handle_command(msg);
                    do_write(command_result);
                    do_read();
                } else if (ec == asio::error::eof) {
                    spdlog::info("Session disconnected {}", boost::uuids::to_string(id_));
                } else {
                    spdlog::error("Read error {}", ec.message());
                }
            });
    }

    void do_write(const std::string& msg) {
        auto self = shared_from_this();
        asio::async_write(socket_, asio::buffer(msg),
            [self](boost::system::error_code ec, std::size_t /*length*/) {
                if (ec) {
                    spdlog::error("Write error {}", ec.message());
                }
            });
    }

    tcp::socket socket_;
    std::array<char, 1024> buffer_;
    boost::uuids::uuid id_;
    std::shared_ptr<JoinStorage> storage_;
};

class Server {
public:
    Server(asio::io_context& io_context, const Options& options)
        : acceptor_(io_context, tcp::endpoint(options.ip_addr, options.port)),
        options_(options),
        storage_(std::make_shared<JoinStorage>()) {
        do_accept();
    }

private:
    void do_accept() {
        spdlog::info("Waiting for connections");
        acceptor_.async_accept(
            [this](std::error_code ec, tcp::socket socket) {
                if (!ec) {
                    spdlog::info("Got new connection {}", socket.remote_endpoint().address().to_string());
                    std::make_shared<Session>(std::move(socket), storage_)->start();
                } else {
                    spdlog::error("Accept error {}", ec.message());
                }
                do_accept();
            });
    }

    tcp::acceptor acceptor_;
    Options options_;
    std::shared_ptr<JoinStorage> storage_;
};

} // server

int main(int argc, char** argv) {
    server::Options options = server::parse_options(argc, argv);

    spdlog::set_level(options.log_level == 0 ? spdlog::level::debug : spdlog::level::warn);

    try {
        asio::io_context io_context;
        server::Server server(io_context, options);
        io_context.run();
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}