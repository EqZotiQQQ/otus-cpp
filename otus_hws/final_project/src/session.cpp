#include "session.hpp"

#include <spdlog/spdlog.h>

#include <chrono>

#include "command_parser.hpp"

UserSession::UserSession(tcp::socket socket, ChatRoom& room, UserManager& user_manager)
    : socket_(std::move(socket)), room_(room), user_manager_(user_manager), id_(boost::uuids::random_generator()()) {
    spdlog::info("New client connected with id {}", boost::uuids::to_string(id_));
}

void UserSession::start() {
    do_read_header();
}

void UserSession::do_read_header() {
    auto self = shared_from_this();

    boost::asio::async_read(socket_,
                            boost::asio::buffer(&incoming_msg_length_, sizeof(incoming_msg_length_)),
                            [this, self](boost::system::error_code ec, std::size_t) {
                                spdlog::info("Got msg from {}", boost::uuids::to_string(id_));
                                if (ec) {
                                    handle_disconnect();
                                    return;
                                }

                                incoming_msg_length_ = ntohl(incoming_msg_length_);

                                if (incoming_msg_length_ == 0 || incoming_msg_length_ > 10 * 1024 * 1024) {
                                    spdlog::warn("Invalid message size");
                                    handle_disconnect();
                                    return;
                                }

                                incoming_buffer_.resize(incoming_msg_length_);
                                do_read_body();
                            });
}

void UserSession::do_read_body() {
    auto self = shared_from_this();

    boost::asio::async_read(socket_, boost::asio::buffer(incoming_buffer_), [this, self](boost::system::error_code ec, std::size_t) {
        if (ec) {
            handle_disconnect();
            return;
        }

        chat::ClientMessage msg;
        if (msg.ParseFromArray(incoming_buffer_.data(), static_cast<int>(incoming_buffer_.size()))) {
            handle_protobuf_message(msg);
        } else {
            spdlog::warn("Failed to parse protobuf message");
        }

        do_read_header();
    });
}

void UserSession::send_protobuf(const chat::ServerMessage& msg) {
    std::string body;
    msg.SerializeToString(&body);

    uint32_t len = htonl(static_cast<uint32_t>(body.size()));

    std::string packet;
    packet.resize(sizeof(len) + body.size());

    memcpy(packet.data(), &len, sizeof(len));
    memcpy(packet.data() + sizeof(len), body.data(), body.size());

    bool write_in_progress = !write_queue_.empty();
    write_queue_.push_back(std::move(packet));

    if (!write_in_progress) {
        do_write();
    }
}

void UserSession::do_write() {
    auto self = shared_from_this();

    boost::asio::async_write(socket_, boost::asio::buffer(write_queue_.front()), [this, self](boost::system::error_code ec, std::size_t) {
        if (ec) {
            handle_disconnect();
            return;
        }

        write_queue_.pop_front();

        if (!write_queue_.empty()) {
            do_write();
        }
    });
}

void UserSession::send_unauth() {
    spdlog::info("Unautharized");
    send_auth_response(false, "Unauthorized");
}

void UserSession::handle_protobuf_message(const chat::ClientMessage& msg) {
    if (msg.has_command()) {
        spdlog::debug("Got command: {}", msg.command().cmd());
        handle_command(msg.command());
        return;
    }

    if (msg.has_chat()) {
        if (state_ != State::Authenticated) {
            send_unauth();
            spdlog::debug("Got message when not authenticated: {}", msg.chat().text());
            return;
        }

        handle_chat(msg.chat());
    }
}

void UserSession::handle_command(const chat::CommandRequest& cmd_request) {
    auto& cmd = cmd_request.cmd();
    if (cmd.empty()) {
        spdlog::warn("Empty command");
        return;
    }

    spdlog::info("Parsing command: {}", cmd);
    Command typed_command = CommandParser::parse(cmd);

    switch (typed_command.type) {
        case CommandType::Register:
            if (typed_command.args.size() < 2) {
                spdlog::error("Not enough arguments to register: {}", cmd);
                return;
            }
            handle_register(typed_command.args[0], typed_command.args[1]);
            break;

        case CommandType::Login:
            if (typed_command.args.size() < 2) {
                spdlog::error("Not enough arguments to login: {}", cmd);
                return;
            }
            handle_login(typed_command.args[0], typed_command.args[1]);
            break;

        case CommandType::History:
            if (state_ != State::Authenticated) {
                send_unauth();
                return;
            }
            room_.deliver_history_proto(shared_from_this());
            break;

        case CommandType::Users:
            if (state_ != State::Authenticated) {
                send_unauth();
                return;
            }
            send_users_list();
            break;

        case CommandType::Help:
            send_help();
            break;

        default:
            send_auth_response(false, fmt::format("Unsupported command {}", cmd));
            spdlog::warn("Unsupported: {}", cmd);
            return;
    }
}

void UserSession::handle_chat(const chat::ChatMessage& chat_msg) {
    chat::ServerMessage server_msg;
    auto* chat = server_msg.mutable_chat();

    chat->set_from(user_name_);
    chat->set_text(chat_msg.text());
    chat->set_timestamp(now_timestamp());

    room_.broadcast_proto(server_msg, shared_from_this());
}

void UserSession::handle_register(const std::string& user_name, const std::string& password) {
    if (user_name.empty() || password.empty()) {
        send_auth_response(false, "Invalid arguments");
        return;
    }

    if (!user_manager_.register_user(user_name, password)) {
        send_auth_response(false, "User already exists");
        return;
    }

    authenticate_success(user_name, password);
}

void UserSession::handle_login(const std::string& user_name, const std::string& password) {
    if (user_name.empty() || password.empty()) {
        send_auth_response(false, "Invalid arguments");
        return;
    }

    if (!user_manager_.is_registered(user_name)) {
        send_auth_response(false, "User not registered");
        return;
    }

    if (user_manager_.is_logined(user_name)) {
        send_auth_response(false, "User already logged in");
        return;
    }

    if (!user_manager_.authenticate({user_name, password})) {
        send_auth_response(false, "Invalid credentials");
        return;
    }

    authenticate_success(user_name, password);
}

void UserSession::authenticate_success(const std::string& username, const std::string& password) {
    user_name_ = username;
    state_ = State::Authenticated;

    user_manager_.authenticate({username, password});
    room_.client_join(shared_from_this());

    send_auth_response(true, "AUTH_OK");

    chat::ServerMessage welcome;
    auto* chat = welcome.mutable_chat();
    chat->set_from("server");
    chat->set_text("Welcome " + username + "!");
    chat->set_timestamp(now_timestamp());

    send_protobuf(welcome);

    room_.deliver_history_proto(shared_from_this());
}

void UserSession::send_auth_response(bool success, const std::string& message) {
    chat::ServerMessage msg;
    auto* auth = msg.mutable_auth();

    auth->set_success(success);
    auth->set_message(message);

    send_protobuf(msg);
}

void UserSession::send_users_list() {
    chat::ServerMessage msg;
    auto* chat = msg.mutable_chat();

    chat->set_from("server");
    chat->set_text("Active users: " + room_.get_logined_users());
    chat->set_timestamp(now_timestamp());

    send_protobuf(msg);
}

void UserSession::send_help() {
    chat::ServerMessage msg;
    auto* chat = msg.mutable_chat();

    chat->set_from("server");
    chat->set_text("Commands: /register or /reg, /login, /history, /users or /u, /help");
    chat->set_timestamp(now_timestamp());

    send_protobuf(msg);
}

void UserSession::handle_disconnect() {
    spdlog::info("Client disconnected {}", user_name_);

    if (!user_name_.empty()) {
        user_manager_.log_out(user_name_);
    }

    room_.client_disconnect(shared_from_this());
    socket_.close();
}

int64_t UserSession::now_timestamp() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

boost::uuids::uuid UserSession::id() const {
    return id_;
}

State UserSession::get_state() const {
    return state_;
}

std::string UserSession::name() const {
    return user_name_;
}
