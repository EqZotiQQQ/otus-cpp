#pragma once

#include <spdlog/spdlog.h>

#include <boost/asio.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <memory>

#include "chat_room.hpp"
#include "schema.pb.h"
#include "user_manager.hpp"

class ChatRoom;
class UserManager;

using boost::asio::ip::tcp;

enum class State { WaitingAuth, Authenticated };

class ClientSessionImpl : public std::enable_shared_from_this<ClientSessionImpl> {
public:
    ClientSessionImpl(std::shared_ptr<SessionTransportInterface> transport, ChatRoom& room, UserManager& user_manager);

    void on_message(const chat::ClientMessage& msg);

    void on_disconnect();

    State state() const;
    std::string name() const;

    std::shared_ptr<SessionTransportInterface> transport();

    ~ClientSessionImpl() {
        spdlog::info("Session destroyed");
    }

    void handle_command(const chat::CommandRequest& cmd_request);

    void handle_chat(const chat::ChatMessage& chat_msg);

    void handle_register(const std::string& user, const std::string& pass);

    void handle_login(const std::string& user, const std::string& pass);

    void authenticate_success(const std::string& user, const std::string& pass);

    void require_auth(auto&& fn);

    void send_auth_response(bool success, const std::string& message);

    void send_users_list();

    void send_help();

    void send_unauth();

    static int64_t now_timestamp();

private:
private:
    std::shared_ptr<SessionTransportInterface> transport_;
    ChatRoom& room_;
    UserManager& user_manager_;

    State state_ = State::WaitingAuth;
    std::string user_name_;
};
