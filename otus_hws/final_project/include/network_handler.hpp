#pragma once

#include <spdlog/spdlog.h>

#include <boost/asio.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <memory>

#include "chat_room.hpp"
#include "schema.pb.h"
#include "transport_interface.hpp"
#include "user_manager.hpp"

class NetworkSession : public SessionTransportInterface, public std::enable_shared_from_this<NetworkSession> {
public:
    NetworkSession(tcp::socket socket, ChatRoom& room, UserManager& user_manager);
    void start();
    void send_protobuf(const chat::ServerMessage& msg) override;
    void close() override;
    boost::uuids::uuid id() const override;

private:
    void do_read_header();
    void do_read_body();
    void do_write();

private:
    tcp::socket socket_;
    boost::uuids::uuid id_;
    std::shared_ptr<ClientSessionImpl> chat_impl;

    ChatRoom& room_;
    UserManager& user_manager_;

    std::vector<char> incoming_buffer_;
    std::deque<std::string> write_queue_;

    uint32_t incoming_len_;
};
