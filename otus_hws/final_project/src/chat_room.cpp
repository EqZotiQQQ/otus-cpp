#include "chat_room.hpp"
#include "server.hpp"

void ChatRoom::join(std::shared_ptr<UserSession> session) {
    sessions_.insert(session);
}

void ChatRoom::leave(std::shared_ptr<UserSession> session) {
    sessions_.erase(session);
}

void ChatRoom::broadcast(const std::string& msg) {
    for (auto& s : sessions_) {
        s->deliver(msg);
    }
}
