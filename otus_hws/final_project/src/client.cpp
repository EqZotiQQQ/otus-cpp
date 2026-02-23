#include "client.hpp"

#include <arpa/inet.h>  // htonl, ntohl
#include <spdlog/spdlog.h>

#include <cstring>

#include "schema.pb.h"

Client::Client(boost::asio::io_context& io, tcp::resolver::results_type endpoints) : io_(io), socket_(io) {
    do_connect(endpoints);
}

void Client::write(const std::string& text) {
    chat::ClientMessage proto_msg;

    if (!text.empty() && text[0] == '/') {
        auto* cmd = proto_msg.mutable_command();
        cmd->set_cmd(text.substr(1));
        spdlog::debug("Sending command: {}", text);
    } else {
        auto* chat_msg = proto_msg.mutable_chat();
        chat_msg->set_text(text);
        spdlog::debug("Sending chat message: {}", text);
    }

    write(proto_msg);
}

void Client::write(const chat::ClientMessage& msg) {
    std::string body;
    msg.SerializeToString(&body);

    uint32_t len = htonl(static_cast<uint32_t>(body.size()));

    std::string packet;
    packet.append(reinterpret_cast<char*>(&len), sizeof(len));
    packet.append(body);

    boost::asio::post(io_, [this, packet]() {
        bool write_in_progress = !write_queue_.empty();
        write_queue_.push_back(packet);
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
    boost::asio::async_read(socket_, boost::asio::buffer(header_), [this](boost::system::error_code ec, std::size_t) {
        if (!ec) {
            uint32_t len = 0;
            std::memcpy(&len, header_.data(), 4);
            len = ntohl(len);

            body_.resize(len);
            do_read_body(len);
        } else {
            spdlog::error("Read header error: {}", ec.message());
            socket_.close();
        }
    });
}

void Client::do_read_body(std::size_t len) {
    boost::asio::async_read(socket_, boost::asio::buffer(body_), [this](boost::system::error_code ec, std::size_t) {
        if (!ec) {
            chat::ServerMessage msg;

            if (msg.ParseFromArray(body_.data(), body_.size())) {
                handle_server_message(msg);
            } else {
                spdlog::error("Failed to parse protobuf");
            }

            do_read();
        } else {
            spdlog::error("Read body error: {}", ec.message());
            socket_.close();
        }
    });
}

void Client::handle_server_message(const chat::ServerMessage& msg) {
    if (msg.has_auth()) {
        spdlog::info("Auth: {}", msg.auth().message());
    }

    if (msg.has_chat()) {
        spdlog::info("[{}] {}", msg.chat().from(), msg.chat().text());
    }

    if (msg.has_users()) {
        spdlog::info("Online users:");
        for (const auto& u : msg.users().users()) {
            spdlog::info(" - {}", u);
        }
    }

    if (msg.has_history()) {
        spdlog::info("Chat history:");
        for (const auto& m : msg.history().messages()) {
            spdlog::info("[{}] {}", m.from(), m.text());
        }
    }
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