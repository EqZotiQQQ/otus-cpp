#pragma once

#include <memory>
#include <set>
#include <string>

#include <spdlog/spdlog.h>

class UserSession;

class ChatRoom {
public:
    void join(std::shared_ptr<UserSession> session);
    void leave(std::shared_ptr<UserSession> session);
    void broadcast(const std::string& msg);

private:
    std::set<std::shared_ptr<UserSession>> sessions_;
};
