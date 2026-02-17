#include "server.hpp"
#include "chat_room.hpp"

#include <spdlog/spdlog.h>


UserSession::UserSession(tcp::socket socket, ChatRoom& room)
    : socket_(std::move(socket)), room_(room),
      id_(boost::uuids::random_generator()()) {
        spdlog::info("New client conected with id {}", boost::uuids::to_string(id_));
      }

void UserSession::start() {
    room_.client_join(shared_from_this());
    do_initial_read();
}

void UserSession::deliver(const std::string& msg) {
    bool write_in_progress = !write_queue_.empty();
    write_queue_.push_back(msg);
    if (!write_in_progress) {
        do_write();
    }
}

void UserSession::do_initial_read() {
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
                room_.client_disconnect(self);
            }
        });
}

void UserSession::do_read() {
    auto self(shared_from_this());
    boost::asio::async_read_until(socket_, buffer_, '\n',
        [this, self](boost::system::error_code ec, std::size_t /*len*/) {
            if (!ec) {
                std::istream is(&buffer_);
                std::string line;
                std::getline(is, line);

                spdlog::info("Client {} ({}) sent {}", user_name_, boost::uuids::to_string(id_), line);

                auto formatted = fmt::format("{}: {}\n", user_name_, line);

                room_.broadcast(formatted);
                do_read();
            } else {
                room_.client_disconnect(self);
            }
        });
}

void UserSession::do_write() {
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
                room_.client_disconnect(self);
            }
        });
}

boost::uuids::uuid UserSession::id() const { return id_; }

Server::Server(boost::asio::io_context& io, short port, size_t history_depth)
    : acceptor_(io, tcp::endpoint(tcp::v4(), port)), room_(ChatRoom{history_depth}) {
    do_accept();
}

void Server::do_accept() {
    acceptor_.async_accept(
        [this](boost::system::error_code ec, tcp::socket socket) {
            if (!ec) {
                spdlog::info("New client conected");
                std::make_shared<UserSession>(std::move(socket), room_)->start();
            }
            do_accept();
        });
}

