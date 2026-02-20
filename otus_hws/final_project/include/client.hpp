#pragma once

#include <deque>
#include <string>

#include <boost/asio.hpp>
#include <boost/asio/ip/address_v4.hpp>

using namespace boost;
using boost::asio::ip::tcp;

class Client {
public:
    Client(boost::asio::io_context& io, tcp::resolver::results_type endpoints);
    void write(const std::string& msg);
    void close();
private:
    void do_connect(const tcp::resolver::results_type& endpoints);
    void do_read();
    void do_write();
private:
    boost::asio::io_context& io_;
    tcp::socket socket_;
    boost::asio::streambuf buffer_;
    std::deque<std::string> write_queue_;
};

