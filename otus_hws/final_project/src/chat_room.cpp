#include "chat_room.hpp"
#include <chrono>
#include <memory>
#include "server.hpp"

ChatRoom::ChatRoom(size_t history_depth):
    history_depth_(history_depth),
    simple_history_{history_depth} {}

void ChatRoom::client_join(std::shared_ptr<UserSession> session) {
    sessions_.insert(session);
}

void ChatRoom::deliver_history_to_client(const std::shared_ptr<UserSession>& session) {
    session->deliver("History...\n");
    if (simple_history_.empty()) {
        session->deliver("History is empty\n");
    } else {
        for (const auto& [stamp, msg]: simple_history_) {
            session->deliver(std::format("{:%Y-%m-%d %H:%M:%S}: {}\n", stamp, msg));
        }
    }
    session->deliver("Live chat...\n");
}

void ChatRoom::client_disconnect(std::shared_ptr<UserSession> session) {
    sessions_.erase(session);
}

void ChatRoom::broadcast(const std::string& msg, const std::chrono::system_clock::time_point& rx_stamp, const std::shared_ptr<UserSession> ignore_user) {
    simple_history_.push_back(std::make_pair(rx_stamp, msg));
    for (auto& s : sessions_) {
        if (!ignore_user || (ignore_user && (ignore_user->id() != s->id()))) {
            if (s->get_state() == State::Authenticated) {
                s->deliver(std::format("{:%Y-%m-%d %H:%M:%S}: {}\n", rx_stamp, msg));
            }
        }
    }
}

void ChatRoom::deliver_to_client(const std::shared_ptr<UserSession>& session, const std::string& msg) {
    session->deliver(std::format("{}\n", msg));
}
