#pragma once

#include "chat_room.hpp"


#include <spdlog/spdlog.h>

#include <boost/asio.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <memory>
#include <deque>
#include <string>

using namespace boost;
using boost::asio::ip::tcp;


class UserSession : public std::enable_shared_from_this<UserSession> {
public:
    explicit UserSession(tcp::socket socket, ChatRoom& room);
    void start();
    void deliver(const std::string& msg);
private:
    void do_read();
    void do_initial_read();
    void do_write();
    boost::uuids::uuid id() const;
private:
    tcp::socket socket_;
    ChatRoom& room_;
    boost::asio::streambuf buffer_;
    std::deque<std::string> write_queue_;
    boost::uuids::uuid id_;
    std::string user_name_;
};


class Server {
public:
    Server(boost::asio::io_context& io, short port, size_t history_depth);
private:
    void do_accept();

private:
    tcp::acceptor acceptor_;
    ChatRoom room_;
};

