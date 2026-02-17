#pragma once

#include <memory>
#include <queue>
#include <set>
#include <string>

#include <boost/circular_buffer.hpp>

#include <spdlog/spdlog.h>

class UserSession;

class ChatRoom {
public:
    explicit ChatRoom(size_t history_depth);
    void client_join(std::shared_ptr<UserSession> session);
    void client_disconnect(std::shared_ptr<UserSession> session);
    void broadcast(const std::string& msg);

private:
    size_t history_depth_;
    std::set<std::shared_ptr<UserSession>> sessions_;
    boost::circular_buffer<std::string> simple_history_;
};
