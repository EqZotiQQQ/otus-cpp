#include "client.hpp"

#include <spdlog/spdlog.h>

Client::Client(boost::asio::io_context& io, tcp::resolver::results_type endpoints) : io_(io), socket_(io) {
    do_connect(endpoints);
}

void Client::write(const std::string& msg) {
    boost::asio::post(io_, [this, msg]() {
        bool write_in_progress = !write_queue_.empty();
        write_queue_.push_back(msg + '\n');
        if (!write_in_progress) {
            do_write();
        }
    });
}

void Client::close() {
    boost::asio::post(io_, [this]() { socket_.close(); });
}

void Client::do_connect(const tcp::resolver::results_type& endpoints) {
    boost::asio::async_connect(socket_, endpoints, [this](boost::system::error_code ec, tcp::endpoint) {
        if (!ec) {
            spdlog::info("Connection established");
            do_read();
        } else {
            spdlog::error("Connect failed: {}", ec.message());
        }
    });
}

void Client::do_read() {
    boost::asio::async_read_until(socket_, buffer_, '\n', [this](boost::system::error_code ec, std::size_t /*len*/) {
        if (!ec) {
            std::istream is(&buffer_);
            std::string line;
            std::getline(is, line);
            spdlog::info("{}", line);
            do_read();
        } else {
            spdlog::error("Read error: {}", ec.message());
            socket_.close();
        }
    });
}

void Client::do_write() {
    boost::asio::async_write(socket_, boost::asio::buffer(write_queue_.front()), [this](boost::system::error_code ec, std::size_t /*len*/) {
        if (!ec) {
            write_queue_.pop_front();
            if (!write_queue_.empty()) {
                do_write();
            }
        } else {
            spdlog::error("Write error: {}", ec.message());
            socket_.close();
        }
    });
}
