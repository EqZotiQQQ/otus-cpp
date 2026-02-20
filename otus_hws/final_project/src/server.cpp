#include "server.hpp"

#include <spdlog/spdlog.h>

#include <boost/smart_ptr/shared_ptr.hpp>
#include <chrono>
#include <memory>

#include "chat_room.hpp"
#include "user_manager.hpp"

UserSession::UserSession(tcp::socket socket, ChatRoom& room, UserManager& user_manager)
    : socket_(std::move(socket)), room_(room), id_(boost::uuids::random_generator()()), user_manager_(user_manager) {
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
    boost::asio::async_read_until(socket_, buffer_, '\n', [this, self](boost::system::error_code ec, std::size_t) {
        if (!ec) {
            auto rx_stamp = std::chrono::system_clock::now();

            std::istream is(&buffer_);
            std::string line;
            std::getline(is, line);

            spdlog::info("Client {} ({}) sent {}", user_name_, boost::uuids::to_string(id_), line);

            if (line.starts_with('/')) {
                auto command = CommandParser::parse(line);
                handle_command(command);
            } else if (state_ == State::Authenticated) {
                handle_message(line, rx_stamp);
            } else {
                deliver("Unauthorized. Use /register or /login\n");
            }

            spdlog::info("Keep listening");
            do_read();
        } else {
            room_.client_disconnect(self);
            user_manager_.log_out(user_name_);
        }
    });
}

void UserSession::handle_message(const std::string& line, const std::chrono::system_clock::time_point& rx_stamp) {
    room_.broadcast(fmt::format("{}: {}", user_name_, line), rx_stamp, shared_from_this());
}

void UserSession::handle_command(const Command& cmd) {
    if (state_ == State::WaitingAuth) {
        handle_auth_command(cmd);
    } else {
        handle_user_command(cmd);
    }
}

void UserSession::handle_user_command(const Command& cmd) {
    switch (cmd.type) {
        case CommandType::History: {
            room_.deliver_history_to_client(shared_from_this());
            break;
        }
        case CommandType::Users: {
            std::string users = room_.get_logined_users();
            deliver(fmt::format("Active users: {}\n", users));
            spdlog::info("User asked to get a list of users: {}", users);
            break;
        }
        case CommandType::Help: {
            deliver(
                "Available commands:\n"
                "/history\n"
                "/users\n"
                "/help\n");
            break;
        }
        default:
            deliver("Unknown command\n");
    }
}

void UserSession::handle_auth_command(const Command& cmd) {
    switch (cmd.type) {
        case CommandType::Register: {
            if (cmd.args.size() != 2) {
                deliver("Usage: /register <username> <password>\n");
                return;
            }

            if (user_manager_.register_user(cmd.args[0], cmd.args[1])) {
                authenticate_success(cmd.args[0]);
            } else {
                deliver("AUTH_FAIL User exists\n");
            }

            break;
        }
        case CommandType::Login: {
            if (cmd.args.size() != 2) {
                deliver("Usage: /login <username> <password>\n");
                return;
            }

            if (!user_manager_.is_registered(cmd.args[0])) {
                deliver("AUTH_FAIL User not registered\n");
                return;
            }

            if (!user_manager_.is_logined(cmd.args[0])) {
                deliver("AUTH_FAIL User already logined\n");
                return;
            }

            if (user_manager_.authenticate({cmd.args[0], cmd.args[1]})) {
                authenticate_success(cmd.args[0]);
            } else {
                deliver("AUTH_FAIL Invalid credentials\n");
            }
            break;
        }
        default: {
            deliver("Unauthorized: /register or /reg or /login <username> <password>\n");
        }
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

boost::uuids::uuid UserSession::id() const {
    return id_;
}

State UserSession::get_state() const {
    return state_;
}

std::string UserSession::name() const {
    return user_name_;
}

Server::Server(boost::asio::io_context& io, short port, size_t history_depth)
    : acceptor_(io, tcp::endpoint(tcp::v4(), port)), room_(ChatRoom{history_depth}) {
    do_accept();
}

void Server::do_accept() {
    acceptor_.async_accept([this](boost::system::error_code ec, tcp::socket socket) {
        if (!ec) {
            spdlog::info("New client conected");
            std::make_shared<UserSession>(std::move(socket), room_, user_manager_)->start();
        }
        do_accept();
    });
}
