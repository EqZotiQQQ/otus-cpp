#pragma once

#include <boost/asio.hpp>
#include <iostream>
#include <thread>
#include <deque>
#include <string>
#include <boost/asio.hpp>
#include <iostream>
#include <memory>
#include <set>
#include <deque>
#include <string>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/asio/ip/address_v4.hpp>
#include <boost/program_options.hpp>
#include <spdlog/spdlog.h>

using namespace boost;
using boost::asio::ip::tcp;

class Client {
public:
    Client(boost::asio::io_context& io, tcp::resolver::results_type endpoints, const std::string& user_name);
    void write(const std::string& msg);
    void close();
private:
    void do_connect(const tcp::resolver::results_type& endpoints);
    void do_read();
    void do_write();
private:
    std::string user_name_;
    boost::asio::io_context& io_;
    tcp::socket socket_;
    boost::asio::streambuf buffer_;
    std::deque<std::string> write_queue_;
};

