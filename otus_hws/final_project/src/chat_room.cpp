#include "chat_room.hpp"

#include <memory>
#include <ranges>

#include "session.hpp"

ChatRoom::ChatRoom(size_t history_depth) : history_depth_(history_depth), simple_history_{history_depth} {
}

void ChatRoom::client_join(std::shared_ptr<UserSession> session) {
    sessions_.insert(session);
}

std::string ChatRoom::get_logined_users() const {
    auto users = sessions_ | std::views::transform([](const auto& ptr) { return ptr->name(); });
    std::string names = std::ranges::fold_left(users, std::string{}, [](std::string acc, const std::string& name) {
        if (!acc.empty())
            acc += ", ";
        return acc + std::format("{}", name);
    });

    if (names.back() == ',') {
        names.pop_back();
    }
    return names;
}

void ChatRoom::client_disconnect(std::shared_ptr<UserSession> session) {
    sessions_.erase(session);
}

void ChatRoom::broadcast_proto(const chat::ServerMessage& msg, const std::shared_ptr<UserSession>& ignore_user) {
    StoredMessage stored{msg.chat().from(), msg.chat().text(), msg.chat().timestamp()};
    simple_history_.push_back(stored);

    for (auto& s : sessions_) {
        if (!ignore_user || (ignore_user && (ignore_user->id() != s->id()))) {
            if (s->get_state() == State::Authenticated) {
                s->send_protobuf(msg);
            }
        }
    }
}

void ChatRoom::deliver_history_proto(std::shared_ptr<UserSession> session) {
    for (const auto& msg : simple_history_) {
        chat::ServerMessage server_msg;
        auto* chat_msg = server_msg.mutable_chat();

        chat_msg->set_from(msg.from);
        chat_msg->set_text(msg.text);
        chat_msg->set_timestamp(msg.timestamp);

        session->send_protobuf(server_msg);
    }
}