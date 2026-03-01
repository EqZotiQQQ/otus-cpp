#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "server/network_handler.hpp"
#include "server/service_handler.hpp"

namespace {

constexpr inline std::chrono::seconds HEARTBEAT_TIMEOUT{30};

}
NetworkSession::NetworkSession(boost::asio::io_context& io, tcp::socket socket, ChatRoom& room, UserManager& user_manager)
    : socket_(std::move(socket)),
      connection_uuid_(boost::uuids::random_generator()()),
      room_(room),
      user_manager_(user_manager),
      last_seen_client_timer_(io) {
}

void NetworkSession::start() {
    spdlog::info("Start network session");

    chat_impl = std::make_shared<ClientSessionImpl>(shared_from_this(), room_, user_manager_);

    do_read_header();
}

void NetworkSession::send_protobuf(const chat::ServerMessage& msg) {
    std::string body;
    msg.SerializeToString(&body);
    uint32_t len = htonl(static_cast<uint32_t>(body.size()));
    std::string packet(sizeof(len) + body.size(), '\0');
    std::memcpy(packet.data(), &len, sizeof(len));
    std::memcpy(packet.data() + sizeof(len), body.data(), body.size());
    bool writing = !write_queue_.empty();
    write_queue_.push_back(std::move(packet));
    if (!writing)
        do_write();
}

boost::uuids::uuid NetworkSession::id() const {
    return connection_uuid_;
}

void NetworkSession::do_read_header() {
    auto self = shared_from_this();
    boost::asio::async_read(socket_,
                            boost::asio::buffer(&incoming_len_, sizeof(incoming_len_)),
                            [this, self](boost::system::error_code ec, std::size_t) {
                                spdlog::debug("Got new message from {}", chat_impl->name());

                                if (ec) {
                                    disconnect();
                                    return;
                                }

                                incoming_len_ = ntohl(incoming_len_);
                                incoming_buffer_.resize(incoming_len_);
                                do_read_body();
                                update_last_seen();
                            });
}

void NetworkSession::do_read_body() {
    auto self = shared_from_this();
    boost::asio::async_read(socket_, boost::asio::buffer(incoming_buffer_), [this, self](boost::system::error_code ec, std::size_t) {
        if (ec) {
            disconnect();
            return;
        }
        chat::ClientMessage msg;
        if (msg.ParseFromArray(incoming_buffer_.data(), static_cast<int>(incoming_buffer_.size()))) {
            chat_impl->on_message(msg);
        }
        do_read_header();
        update_last_seen();
    });
}

void NetworkSession::update_last_seen() {
    set_clinet_silent_timeout_timer();
    last_seen_client_stamp_ns_ = std::chrono::system_clock::now().time_since_epoch().count();
}

void NetworkSession::disconnect() {
    if (!is_alive_) {
        return;
    }
    spdlog::warn("Disconnecting client {} from server due to not seen for a while", boost::uuids::to_string(connection_uuid_));
    last_seen_client_timer_.cancel();
    chat_impl->on_disconnect();
    socket_.close();
    is_alive_ = false;
}

void NetworkSession::set_clinet_silent_timeout_timer() {
    last_seen_client_timer_.cancel();
    last_seen_client_timer_.expires_after(HEARTBEAT_TIMEOUT);

    auto self = shared_from_this();

    last_seen_client_timer_.async_wait([self](const boost::system::error_code& ec) {
        if (ec == boost::asio::error::operation_aborted) {
            return;
        }

        if (!ec) {
            self->disconnect();
        }
    });
}

void NetworkSession::unsed_clinet_silent_timeout_timer() {
    last_seen_client_timer_.cancel();
}

void NetworkSession::do_write() {
    auto self = shared_from_this();
    boost::asio::async_write(socket_, boost::asio::buffer(write_queue_.front()), [this, self](boost::system::error_code ec, std::size_t) {
        if (ec) {
            self->disconnect();
            return;
        }
        write_queue_.pop_front();
        if (!write_queue_.empty()) {
            do_write();
        }
    });
}
