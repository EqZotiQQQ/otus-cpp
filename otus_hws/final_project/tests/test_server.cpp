#include <boost/random/uniform_int_distribution.hpp>

#include "spdlog/spdlog.h"
#define BOOST_TEST_MODULE ClientSessionTests
#include <boost/test/included/unit_test.hpp>

#include "chat_room.hpp"
#include "command_parser.hpp"
#include "service_handler.hpp"
#include "transport_interface.hpp"
#include "user_manager.hpp"

// Mock Transport
struct MockTransport : public SessionTransportInterface {
    std::vector<chat::ServerMessage> sent_messages;

    void send_protobuf(const chat::ServerMessage& msg) override {
        spdlog::critical("Message sent");
        sent_messages.push_back(msg);
    }
    boost::uuids::uuid id() const override {
        return boost::uuids::uuid{};
    }
    void close() override {
        return;
    }
};

// Mock ChatRoom
struct MockChatRoom : public ChatRoom {
    std::vector<std::shared_ptr<ClientSessionImpl>> joined_clients;

    MockChatRoom() : ChatRoom(10) {
    }

    void client_join(const std::shared_ptr<ClientSessionImpl>& session) {
        joined_clients.push_back(session);
    }

    void client_disconnect(const std::shared_ptr<ClientSessionImpl>&) {
    }

    void broadcast_proto(const chat::ServerMessage&, const std::shared_ptr<ClientSessionImpl>&) {
    }
};

// Simple UserManager
struct MockUserManager : public UserManager {
    std::map<std::string, std::string> users;

    bool register_user(const std::string& user, const std::string& pass) {
        if (users.count(user))
            return false;
        users[user] = pass;
        return true;
    }

    bool authenticate(const std::pair<std::string, std::string>& creds) {
        auto it = users.find(creds.first);
        return it != users.end() && it->second == creds.second;
    }

    void log_out(const std::string& user) {
        (void) user;
    }
};

BOOST_AUTO_TEST_CASE(test_register_and_login) {
    auto transport = std::make_shared<MockTransport>();
    MockChatRoom room;
    MockUserManager manager;

    // Fine reg
    auto session = std::make_shared<ClientSessionImpl>(transport, room, manager);

    chat::ClientMessage msg;
    msg.mutable_command()->set_cmd("register testuser testpass");
    session->on_message(msg);

    BOOST_CHECK(session->state() == State::Authenticated);
    BOOST_CHECK(session->name() == "testuser");
    BOOST_CHECK(transport->sent_messages.size() == 2);

    BOOST_CHECK(room.users_size() == 1);

    chat::CommandRequest users;
    msg.mutable_command()->set_cmd("users");
    session->on_message(msg);
    auto last = transport->sent_messages.back();
    BOOST_CHECK(last.chat().text() == "Active users: testuser");

    session->on_disconnect();
    BOOST_CHECK(room.users_size() == 0);

    msg.mutable_command()->set_cmd("users");
    session->on_message(msg);
    last = transport->sent_messages.back();
    BOOST_CHECK(last.chat().text() == "Active users: ");

    // Login with wrong password
    auto session2 = std::make_shared<ClientSessionImpl>(transport, room, manager);
    chat::CommandRequest login;
    msg.mutable_command()->set_cmd("login testuser wrongpass");
    session2->on_message(msg);
    BOOST_CHECK(session2->state() != State::Authenticated);
}

BOOST_AUTO_TEST_CASE(test_send_help_and_users) {
    auto transport = std::make_shared<MockTransport>();
    MockChatRoom room;
    MockUserManager manager;

    auto session = std::make_shared<ClientSessionImpl>(transport, room, manager);

    // Help
    chat::ClientMessage msg;
    chat::CommandRequest help;
    msg.mutable_command()->set_cmd("help");
    session->on_message(msg);

    BOOST_CHECK(!transport->sent_messages.empty());
    auto last = transport->sent_messages.back();
    BOOST_CHECK(last.chat().text() == "Commands: /register, /login, /history, /users, /help");

    // Users list without auth
    chat::CommandRequest users;
    msg.mutable_command()->set_cmd("users");
    session->on_message(msg);
    last = transport->sent_messages.back();
    BOOST_CHECK(last.auth().success() == false);
}
