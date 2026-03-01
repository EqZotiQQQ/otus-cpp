#pragma once

#include <boost/circular_buffer.hpp>
#include <memory>
#include <set>
#include <string>

#include "schema.pb.h"

class Session;
class SessionTransportInterface;
class ClientSessionImpl;

struct StoredMessage {
    std::string from;
    std::string text;
    int64_t timestamp;
};

struct WeakPtrHash {
    template <typename T>
    std::size_t operator()(const std::weak_ptr<T>& wp) const noexcept {
        if (auto sp = wp.lock()) {
            return std::hash<std::shared_ptr<T>>{}(sp);
        }
        return 0;
    }
};

struct WeakPtrEqual {
    template <typename T>
    bool operator()(const std::weak_ptr<T>& a, const std::weak_ptr<T>& b) const noexcept {
        return !a.owner_before(b) && !b.owner_before(a);
    }
};

class ChatRoom {
public:
    explicit ChatRoom(size_t history_depth);
    void client_join(const std::shared_ptr<ClientSessionImpl>& session);
    void client_disconnect(const std::shared_ptr<ClientSessionImpl>& session);
    void broadcast_proto(const chat::ServerMessage& msg, const std::shared_ptr<ClientSessionImpl>& ignore_user);
    void deliver_history_proto(const std::shared_ptr<ClientSessionImpl>& session);

    [[nodiscard]] std::string get_logined_users() const;
    [[nodiscard]] size_t users_size() const;
    [[nodiscard]] size_t hist_size() const;

private:
    size_t history_depth_;
    mutable std::mutex mutex_;
    boost::circular_buffer<StoredMessage> simple_history_;
    std::unordered_set<std::weak_ptr<ClientSessionImpl>, WeakPtrHash, WeakPtrEqual> sessions_;
};
