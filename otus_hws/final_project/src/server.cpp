#include "server.hpp"

#include <arpa/inet.h>
#include <spdlog/spdlog.h>

#include <chrono>
#include <stdexcept>

#include "command_parser.hpp"
#include "network_handler.hpp"
#include "service_handler.hpp"

using namespace std::chrono;

Server::Server(boost::asio::io_context& io, short port, size_t history_depth)
    : acceptor_(io, tcp::endpoint(tcp::v4(), port)), room_(ChatRoom{history_depth}) {
    do_accept();
}

void Server::do_accept() {
    acceptor_.async_accept([this](boost::system::error_code ec, tcp::socket socket) {
        spdlog::info("New client arrived!");
        if (!ec) {
            std::make_shared<NetworkSession>(std::move(socket), room_, user_manager_)->start();
        }

        do_accept();
    });
}
