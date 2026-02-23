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

enum class State { WaitingAuth, Authenticated };

class Session : public std::enable_shared_from_this<Session> {
public:
    explicit Session(tcp::socket socket, ChatRoom& room, UserManager& user_manager);

    void start();

    void send_protobuf(const chat::ServerMessage& msg);

    State get_state() const;
    boost::uuids::uuid id() const;
    std::string name() const;

private:
    void do_read_header();
    void do_read_body();
    void handle_protobuf_message(const chat::ClientMessage& msg);

    void handle_command(const chat::CommandRequest& cmd);
    void handle_chat(const chat::ChatMessage& chat_msg);

    void handle_register(const std::string& user_name, const std::string& password);
    void handle_login(const std::string& user_name, const std::string& password);

    void send_auth_response(bool success, const std::string& message);
    void send_users_list();
    void send_help();
    void authenticate_success(const std::string& username, const std::string& password);
    void send_unauth();
    void do_write();

    void handle_disconnect();
    static int64_t now_timestamp();

private:
    tcp::socket socket_;
    ChatRoom& room_;
    UserManager& user_manager_;

    boost::uuids::uuid id_;
    std::string user_name_;

    State state_ = State::WaitingAuth;

    uint32_t incoming_msg_length_ = 0;
    std::vector<char> incoming_buffer_;

    std::deque<std::string> write_queue_;
};
