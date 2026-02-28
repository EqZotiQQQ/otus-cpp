#pragma once

#include <boost/asio.hpp>
#include <boost/asio/high_resolution_timer.hpp>
#include <boost/asio/ip/address_v4.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/system_timer.hpp>
#include <deque>
#include <memory>
#include <string>

#include "schema.pb.h"

using namespace boost;
using boost::asio::ip::tcp;

class Client : public std::enable_shared_from_this<Client> {
public:
    Client(boost::asio::io_context& io);
    void start(tcp::resolver::results_type endpoints);
    void write(const std::string& msg);
    void write(const chat::ClientMessage& msg);
    void close();

private:
    void do_connect(const tcp::resolver::results_type& endpoints);
    void do_read();
    void do_write();
    void do_read_body(std::size_t len);
    void handle_server_message(const chat::ServerMessage& msg);

    void start_heartbeat_timer();
    void send_heartbeat();
    void reset_heartbeat_timer();
    void cancel_timer();
    void set_heartbeat_timeout_timer();

private:
    boost::asio::io_context& io_;
    tcp::socket socket_;

    std::deque<std::string> write_queue_;

    std::array<char, 4> header_;
    std::vector<char> body_;
    boost::asio::streambuf buffer_;

    boost::asio::steady_timer heartbeat_timer_;
    boost::asio::steady_timer heartbeat_timeout_timer_;
    bool connection_established_ = false;
    int64_t server_last_seen_ = 0;
};
