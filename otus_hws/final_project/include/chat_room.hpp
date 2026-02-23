#pragma once

#include <boost/circular_buffer.hpp>
#include <chrono>
#include <memory>
#include <queue>
#include <ranges>
#include <set>
#include <string>

#include "schema.pb.h"

class UserSession;

struct StoredMessage {
    std::string from;
    std::string text;
    int64_t timestamp;
};

class ChatRoom {
public:
    explicit ChatRoom(size_t history_depth);
    void client_join(std::shared_ptr<UserSession> session);
    void client_disconnect(std::shared_ptr<UserSession> session);
    // void broadcast(const std::string& msg,
    //                const std::chrono::system_clock::time_point& rx_stamp,
    //                const std::shared_ptr<UserSession> muted_user = nullptr);
    void broadcast_proto(const chat::ServerMessage& msg, const std::shared_ptr<UserSession>& ignore_user);
    // void deliver_to_client(const std::shared_ptr<UserSession>& session, const std::string& msg);

    std::string get_logined_users() const;
    // void deliver_history_to_client(const std::shared_ptr<UserSession>& session);

    void deliver_history_proto(std::shared_ptr<UserSession> session);

private:
    size_t history_depth_;
    std::set<std::shared_ptr<UserSession>> sessions_;
    boost::circular_buffer<StoredMessage> simple_history_;
};
