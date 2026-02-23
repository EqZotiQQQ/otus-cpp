#pragma once

#include <boost/circular_buffer.hpp>
#include <memory>
#include <set>
#include <string>

#include "schema.pb.h"

class Session;

struct StoredMessage {
    std::string from;
    std::string text;
    int64_t timestamp;
};

class ChatRoom {
public:
    explicit ChatRoom(size_t history_depth);
    void client_join(std::shared_ptr<Session> session);
    void client_disconnect(std::shared_ptr<Session> session);
    void broadcast_proto(const chat::ServerMessage& msg, const std::shared_ptr<Session>& ignore_user);
    std::string get_logined_users() const;
    void deliver_history_proto(std::shared_ptr<Session> session);

private:
    size_t history_depth_;
    std::set<std::shared_ptr<Session>> sessions_;
    boost::circular_buffer<StoredMessage> simple_history_;
};
