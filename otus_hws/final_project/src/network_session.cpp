#include "network_handler.hpp"
#include "service_handler.hpp"

NetworkSession::NetworkSession(tcp::socket socket, ChatRoom& room, UserManager& user_manager)
    : socket_(std::move(socket)), id_(boost::uuids::random_generator()()), room_(room), user_manager_(user_manager) {
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

void NetworkSession::close() {
    socket_.close();
}

boost::uuids::uuid NetworkSession::id() const {
    return id_;
}

void NetworkSession::do_read_header() {
    auto self = shared_from_this();
    boost::asio::async_read(socket_,
                            boost::asio::buffer(&incoming_len_, sizeof(incoming_len_)),
                            [this, self](boost::system::error_code ec, std::size_t) {
                                spdlog::debug("Got new message from {}", chat_impl->name());
                                if (ec) {
                                    spdlog::debug("Got new message from {} DC: {}", chat_impl->name(), ec.message());
                                    chat_impl->on_disconnect();
                                    return;
                                }
                                incoming_len_ = ntohl(incoming_len_);
                                incoming_buffer_.resize(incoming_len_);
                                do_read_body();
                            });
}

void NetworkSession::do_read_body() {
    auto self = shared_from_this();
    boost::asio::async_read(socket_, boost::asio::buffer(incoming_buffer_), [this, self](boost::system::error_code ec, std::size_t) {
        if (ec) {
            chat_impl->on_disconnect();
            return;
        }
        chat::ClientMessage msg;
        if (msg.ParseFromArray(incoming_buffer_.data(), static_cast<int>(incoming_buffer_.size()))) {
            chat_impl->on_message(msg);
        }
        do_read_header();
    });
}

void NetworkSession::do_write() {
    auto self = shared_from_this();
    boost::asio::async_write(socket_, boost::asio::buffer(write_queue_.front()), [this, self](boost::system::error_code ec, std::size_t) {
        if (ec) {
            chat_impl->on_disconnect();
            return;
        }
        write_queue_.pop_front();
        if (!write_queue_.empty())
            do_write();
    });
}
