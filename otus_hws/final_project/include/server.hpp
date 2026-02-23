#pragma once

#include <boost/asio.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <chrono>
#include <deque>
#include <memory>
#include <string>
#include <vector>

#include "chat_room.hpp"
#include "schema.pb.h"
#include "user_manager.hpp"

using boost::asio::ip::tcp;

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