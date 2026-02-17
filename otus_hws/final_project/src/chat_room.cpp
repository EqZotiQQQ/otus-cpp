#include "chat_room.hpp"
#include "server.hpp"

ChatRoom::ChatRoom(size_t history_depth):
    history_depth_(history_depth),
    simple_history_{history_depth} {}

void ChatRoom::client_join(std::shared_ptr<UserSession> session) {
    sessions_.insert(session);
    session->deliver("History...");
    for (const auto& msg: simple_history_) {
        session->deliver(msg);
    }
    session->deliver("Live chat...");
}

void ChatRoom::client_disconnect(std::shared_ptr<UserSession> session) {
    sessions_.erase(session);
}

void ChatRoom::broadcast(const std::string& msg) {
    simple_history_.push_back(msg);
    for (auto& s : sessions_) {
        s->deliver(msg);
    }
}
