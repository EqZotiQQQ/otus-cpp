#pragma once

#include <boost/asio.hpp>
#include <boost/asio/ip/address_v4.hpp>
#include <deque>
#include <string>

#include "schema.pb.h"

using namespace boost;
using boost::asio::ip::tcp;

class Client {
public:
    Client(boost::asio::io_context& io, tcp::resolver::results_type endpoints);
    void write(const std::string& msg);
    void write(const chat::ClientMessage& msg);
    void close();

private:
    void do_connect(const tcp::resolver::results_type& endpoints);
    void do_read();
    void do_write();
    void do_read_body(std::size_t len);
    void handle_server_message(const chat::ServerMessage& msg);

private:
    std::array<char, 4> header_;
    std::vector<char> body_;
    boost::asio::io_context& io_;
    tcp::socket socket_;
    boost::asio::streambuf buffer_;
    std::deque<std::string> write_queue_;
};
