#include <chrono>

#include "command_parser.hpp"
#include "service_handler.hpp"
#include "spdlog/spdlog.h"
#include "transport_interface.hpp"
#include "user_manager.hpp"

ClientSessionImpl::ClientSessionImpl(std::shared_ptr<SessionTransportInterface> transport, ChatRoom& room, UserManager& user_manager)
    : transport_(std::move(transport)), room_(room), user_manager_(user_manager) {
}

void ClientSessionImpl::on_message(const chat::ClientMessage& msg) {
    if (msg.has_heartbeet()) {
        send_heartbeat_response();
    } else if (msg.has_command()) {
        handle_command(msg.command());
        return;
    } else if (msg.has_chat()) {
        if (state_ != State::Authenticated) {
            send_unauth();
            return;
        }
        handle_chat(msg.chat());
    }
    last_seen_ = std::chrono::system_clock::now().time_since_epoch().count();
}

void ClientSessionImpl::on_disconnect() {
    spdlog::info("Client disconnected {}", user_name_);
    user_manager_.log_out(user_name_);
    room_.client_disconnect(shared_from_this());
}

State ClientSessionImpl::state() const {
    return state_;
}

std::string ClientSessionImpl::name() const {
    return user_name_;
}

std::shared_ptr<SessionTransportInterface> ClientSessionImpl::transport() const {
    return transport_;
}

void ClientSessionImpl::handle_command(const chat::CommandRequest& cmd_request) {
    auto cmd = cmd_request.cmd();
    spdlog::info("Handle command: {}", cmd);
    if (cmd.empty()) {
        spdlog::warn("Empty command", cmd);
        return;
    }
    Command parsed_command = CommandParser::parse(cmd);
    switch (parsed_command.type) {
        case CommandType::Register:
            if (parsed_command.args.size() == 2) {
                handle_register(parsed_command.args[0], parsed_command.args[1]);
            } else {
                spdlog::warn("Bad args for register: {}", cmd);
            }
            break;
        case CommandType::Login:
            if (parsed_command.args.size() == 2) {
                handle_login(parsed_command.args[0], parsed_command.args[1]);
            } else {
                spdlog::warn("Bad args for login: {}", cmd);
            }
            break;
        case CommandType::History:
            wrap_require_auth([this] { room_.deliver_history_proto(shared_from_this()); });
            break;
        case CommandType::Users:
            wrap_require_auth([this] { send_users_list(); });
            break;
        case CommandType::Help:
            send_help();
            break;
        default:
            send_auth_response(false, "Unsupported command");
            break;
    }
}

void ClientSessionImpl::handle_chat(const chat::ChatMessage& chat_msg) {
    chat::ServerMessage server_msg;
    auto* chat = server_msg.mutable_chat();

    chat->set_from(user_name_);
    chat->set_text(chat_msg.text());
    chat->set_timestamp(now_timestamp());

    room_.broadcast_proto(server_msg, shared_from_this());
}

void ClientSessionImpl::handle_register(const std::string& user, const std::string& pass) {
    spdlog::info("Register user {}:{}", user, pass);

    if (!user_manager_.register_user(user, pass)) {
        send_auth_response(false, "User already exists");
        return;
    }

    authenticate_success(user, pass);
}

void ClientSessionImpl::handle_login(const std::string& user, const std::string& pass) {
    spdlog::info("Login user {}:{}", user, pass);

    if (!user_manager_.authenticate({user, pass})) {
        send_auth_response(false, "Invalid credentials");
        return;
    }

    authenticate_success(user, pass);
}

void ClientSessionImpl::authenticate_success(const std::string& user, const std::string& pass) {
    user_name_ = user;
    state_ = State::Authenticated;

    room_.client_join(shared_from_this());

    send_auth_response(true, "AUTH_OK");

    chat::ServerMessage welcome;
    auto* chat = welcome.mutable_chat();
    chat->set_from("server");
    chat->set_text(user + " Welcome to the club buddy!");
    chat->set_timestamp(now_timestamp());

    transport_->send_protobuf(welcome);
}

void ClientSessionImpl::wrap_require_auth(auto&& fn) {
    if (state_ != State::Authenticated) {
        send_unauth();
        return;
    }
    fn();
}

void ClientSessionImpl::send_auth_response(bool success, const std::string& message) {
    chat::ServerMessage msg;
    auto* auth = msg.mutable_auth();

    auth->set_success(success);
    auth->set_message(message);

    transport_->send_protobuf(msg);
}

void ClientSessionImpl::send_users_list() {
    spdlog::debug("User requested for a user list");

    chat::ServerMessage msg;
    auto* chat = msg.mutable_chat();

    chat->set_from("server");
    chat->set_text("Active users: " + room_.get_logined_users());
    chat->set_timestamp(now_timestamp());

    transport_->send_protobuf(msg);
}

void ClientSessionImpl::send_help() {
    spdlog::debug("User requested for a help message");

    chat::ServerMessage msg;
    auto* chat = msg.mutable_chat();

    chat->set_from("server");
    chat->set_text(
        "Commands: /register ${name} ${password}, /reg  ${name} ${password}, /login ${name} ${password}, /history, /users, /help");
    chat->set_timestamp(now_timestamp());

    transport_->send_protobuf(msg);
}

void ClientSessionImpl::send_unauth() {
    send_auth_response(false, "Unauthorized");
}

int64_t ClientSessionImpl::now_timestamp() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

void ClientSessionImpl::send_heartbeat_response() {
    chat::ServerMessage server_msg;
    auto* hb_msg = server_msg.mutable_heartbeet();

    hb_msg->set_timestamp(std::chrono::system_clock::now().time_since_epoch().count());

    transport_->send_protobuf(server_msg);
}
