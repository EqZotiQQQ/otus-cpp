#include "service.hpp"

namespace po = boost::program_options;

void ChatRoom::join(std::shared_ptr<Session> session) {
    sessions_.insert(session);
}

void ChatRoom::leave(std::shared_ptr<Session> session) {
    sessions_.erase(session);
}

void ChatRoom::broadcast(const std::string& msg) {
    for (auto& s : sessions_) {
        s->deliver(msg);
    }
}

Session::Session(tcp::socket socket, ChatRoom& room)
    : socket_(std::move(socket)), room_(room),
      id_(boost::uuids::random_generator()()) {
        spdlog::info("New client conected with id {}", boost::uuids::to_string(id_));
      }

void Session::start() {
    room_.join(shared_from_this());
    do_initial_read();
}

void Session::deliver(const std::string& msg) {
    bool write_in_progress = !write_queue_.empty();
    write_queue_.push_back(msg);
    if (!write_in_progress) {
        do_write();
    }
}

void Session::do_initial_read() {
    auto self(shared_from_this());
    boost::asio::async_read_until(socket_, buffer_, '\n',
        [this, self](boost::system::error_code ec, std::size_t /*len*/) {
            if (!ec) {
                std::istream is(&buffer_);
                std::string line;
                std::getline(is, line);

                user_name_ = line;

                room_.broadcast(fmt::format("User {} has joined the room!\n", user_name_));
                do_read();
            } else {
                room_.leave(self);
            }
        });
}

void Session::do_read() {
    auto self(shared_from_this());
    boost::asio::async_read_until(socket_, buffer_, '\n',
        [this, self](boost::system::error_code ec, std::size_t /*len*/) {
            if (!ec) {
                std::istream is(&buffer_);
                std::string line;
                std::getline(is, line);

                // Broadcast to everyone
                spdlog::info("Client {} ({}) sent {}", user_name_, boost::uuids::to_string(id_), line);

                room_.broadcast(fmt::format("{}: {}\n", user_name_, line));
                do_read();
            } else {
                room_.leave(self);
            }
        });
}

void Session::do_write() {
    auto self(shared_from_this());
    boost::asio::async_write(socket_,
        boost::asio::buffer(write_queue_.front()),
        [this, self](boost::system::error_code ec, std::size_t /*len*/) {
            if (!ec) {
                write_queue_.pop_front();
                if (!write_queue_.empty()) {
                    do_write();
                }
            } else {
                room_.leave(self);
            }
        });
}

boost::uuids::uuid Session::id() const { return id_; }

Options parse_options(int argc, char* argv[]) {
    Options opts;

    std::string ip_as_str;

    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "this message")
        ("port,p", po::value<uint16_t>(&opts.port)->default_value(12345), "port")
        ("log-level,l", po::value<uint8_t>(&opts.log_level)->default_value(1), "0-info+, 1-warn+")
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

Server::Server(boost::asio::io_context& io, short port)
    : acceptor_(io, tcp::endpoint(tcp::v4(), port)) {
    do_accept();
}

void Server::do_accept() {
    acceptor_.async_accept(
        [this](boost::system::error_code ec, tcp::socket socket) {
            if (!ec) {
                spdlog::info("New client conected");
                std::make_shared<Session>(std::move(socket), room_)->start();
            }
            do_accept();
        });
}


int main(int argc, char* argv[]) {
    Options options = parse_options(argc, argv);
    try {
        boost::asio::io_context io;

        Server server(io, options.port);

        spdlog::info("Chat server started on port {}", options.port);

        io.run();
    }
    catch (std::exception& e) {
        spdlog::error("Exception {}", e.what());
    }
}
