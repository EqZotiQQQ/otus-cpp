#include "server.hpp"
#include "chat_room.hpp"
#include "user_manager.hpp"

#include <spdlog/spdlog.h>
#include <chrono>


UserSession::UserSession(tcp::socket socket, ChatRoom& room, UserManager& user_manager)
    : socket_(std::move(socket)), room_(room),
      id_(boost::uuids::random_generator()()),
      user_manager_(user_manager) {
        spdlog::info("New client conected with id {}", boost::uuids::to_string(id_));
      }

void UserSession::start() {
    room_.client_join(shared_from_this());
    do_read();
}

void UserSession::deliver(const std::string& msg) {
    bool write_in_progress = !write_queue_.empty();
    write_queue_.push_back(msg);
    if (!write_in_progress) {
        do_write();
    }
}


void UserSession::do_read() {
    auto self(shared_from_this());
    boost::asio::async_read_until(socket_, buffer_, '\n',
        [this, self](boost::system::error_code ec, std::size_t) {
            if (!ec) {
                std::istream is(&buffer_);
                std::string line;
                std::getline(is, line);

                spdlog::info("Client {} ({}) sent {}", user_name_, boost::uuids::to_string(id_), line);

                if (state_ == State::WaitingAuth) {
                    handle_auth(line);
                } else {
                    auto rx_stamp = std::chrono::system_clock::now();
                    handle_message(line, rx_stamp);
                }

                do_read();
            } else {
                room_.client_disconnect(self);
            }
        });
}

void UserSession::handle_message(const std::string& line, const std::chrono::system_clock::time_point& rx_stamp) {
    room_.broadcast(fmt::format("{}: {}", user_name_, line), rx_stamp);
}

void UserSession::handle_auth(const std::string& line) {
    std::istringstream iss(line);
    std::string cmd, username, password;
    iss >> cmd >> username >> password;

    if (cmd == "REGISTER") {
        if (user_manager_.register_user(UserData{username, password})) {
            authenticate_success(username);
        } else {
            deliver("AUTH_FAIL User exists");
        }
    }
    else if (cmd == "LOGIN") {
        if (user_manager_.authenticate(UserData{username, password})) {
            authenticate_success(username);
        } else {
            deliver("AUTH_FAIL Invalid credentials");
        }
    }
    else {
        deliver("AUTH_FAIL Unknown command");
    }
}

void UserSession::authenticate_success(const std::string& username) {
    user_name_ = username;
    state_ = State::Authenticated;

    deliver("AUTH_OK\n");
    room_.client_join(shared_from_this());
    room_.deliver_to_client(shared_from_this(), fmt::format("{}, welcome to the club, buddy!", user_name_));
    room_.deliver_history_to_client(shared_from_this());
    room_.broadcast(fmt::format("{} has joined the room!", user_name_), std::chrono::system_clock::now());
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
                std::make_shared<UserSession>(std::move(socket), room_, user_manager_)->start();
            }
            do_accept();
        });
}

