#pragma once

#include "chat_room.hpp"
#include "user_manager.hpp"


#include <boost/asio.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <memory>
#include <deque>
#include <string>

using namespace boost;
using boost::asio::ip::tcp;

enum class State {
    WaitingAuth,
    Authenticated
};


class UserSession : public std::enable_shared_from_this<UserSession> {
public:
    explicit UserSession(tcp::socket socket, ChatRoom& room, UserManager& user_manager);
    void start();
    void deliver(const std::string& msg);
    State get_state() const {
        return state_;
    }
private:
    void do_read();
    void handle_auth(const std::string& line);
    void do_write();
    void authenticate_success(const std::string& username);
    void handle_message(const std::string& line, const std::chrono::system_clock::time_point& rx_stamp);
    boost::uuids::uuid id() const;
private:
    tcp::socket socket_;
    ChatRoom& room_;
    boost::asio::streambuf buffer_;
    std::deque<std::string> write_queue_;
    boost::uuids::uuid id_;
    std::string user_name_;
    UserManager& user_manager_;
    State state_ = State::WaitingAuth;
};


class Server {
public:
    Server(boost::asio::io_context& io, short port, size_t history_depth);
private:
    void do_accept();

private:
    tcp::acceptor acceptor_;
    ChatRoom room_;
    UserManager user_manager_;
};

