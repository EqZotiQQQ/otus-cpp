#include "chat_room.hpp"

#include <memory>
#include <ranges>

#include "network_handler.hpp"
#include "service_handler.hpp"

ChatRoom::ChatRoom(size_t history_depth) : history_depth_(history_depth), simple_history_{history_depth} {
}

void ChatRoom::client_join(const std::shared_ptr<ClientSessionImpl>& session) {
    std::scoped_lock lock(mutex_);
    sessions_.insert(session->weak_from_this());
}

void ChatRoom::client_disconnect(const std::shared_ptr<ClientSessionImpl>& session) {
    std::scoped_lock lock(mutex_);
    sessions_.erase(session->weak_from_this());
}

std::string ChatRoom::get_logined_users() const {
    std::scoped_lock lock(mutex_);
    auto users_view = sessions_ | std::views::filter([](const auto& weak) { return !weak.expired(); }) |
                      std::views::transform([](const auto& weak) { return weak.lock()->name(); }) |
                      std::views::filter([](const std::string& name) { return !name.empty(); });
    std::string result;
    for (const auto& name : users_view) {
        if (!result.empty())
            result += ", ";
        result += name;
    }
    return result;
}

void ChatRoom::broadcast_proto(const chat::ServerMessage& msg, const std::shared_ptr<ClientSessionImpl>& ignore_user) {
    std::vector<std::shared_ptr<ClientSessionImpl>> alive;
    {
        std::scoped_lock lock(mutex_);
        if (msg.has_chat()) {
            simple_history_.push_back({msg.chat().from(), msg.chat().text(), msg.chat().timestamp()});
        }
        for (auto it = sessions_.begin(); it != sessions_.end();) {
            if (auto s = it->lock()) {
                alive.push_back(std::move(s));
                ++it;
            } else {
                it = sessions_.erase(it);
            }
        }
    }

    for (auto& s : alive) {
        if (ignore_user && s == ignore_user) {
            continue;
        }

        if (s->state() == State::Authenticated) {
            s->transport()->send_protobuf(msg);
        }
    }
}

void ChatRoom::deliver_history_proto(const std::shared_ptr<ClientSessionImpl>& session) {
    std::vector<StoredMessage> history_copy;

    {
        std::scoped_lock lock(mutex_);
        history_copy.assign(simple_history_.begin(), simple_history_.end());
    }

    for (const auto& msg : history_copy) {
        chat::ServerMessage server_msg;
        auto* chat_msg = server_msg.mutable_chat();

        chat_msg->set_from(msg.from);
        chat_msg->set_text(msg.text);
        chat_msg->set_timestamp(msg.timestamp);

        session->transport()->send_protobuf(server_msg);
    }
}

size_t ChatRoom::users_size() const {
    return sessions_.size();
}

size_t ChatRoom::hist_size() const {
    return simple_history_.size();
}
