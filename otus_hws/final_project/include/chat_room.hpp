#pragma once

#include <chrono>
#include <memory>
#include <queue>
#include <set>
#include <string>

#include <boost/circular_buffer.hpp>

class UserSession;

class ChatRoom {
public:
    explicit ChatRoom(size_t history_depth);
    void client_join(std::shared_ptr<UserSession> session);
    void client_disconnect(std::shared_ptr<UserSession> session);
    void broadcast(const std::string& msg, const std::chrono::system_clock::time_point& rx_stamp);
    void deliver_to_client(const std::shared_ptr<UserSession>& session, const std::string& msg);
    void deliver_history_to_client(const std::shared_ptr<UserSession>& session);

private:
    size_t history_depth_;
    std::set<std::shared_ptr<UserSession>> sessions_;
    boost::circular_buffer<std::pair<std::chrono::system_clock::time_point, std::string>> simple_history_;
};
