#include <boost/asio.hpp>
#include <iostream>
#include <memory>
#include <set>
#include <deque>
#include <string>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/asio/ip/address_v4.hpp>
#include <boost/program_options.hpp>
#include <spdlog/spdlog.h>

using namespace boost;
using boost::asio::ip::tcp;

struct Options {
    uint16_t port;
    boost::asio::ip::address_v4 ip_addr;
    uint8_t log_level;
};


// Forward declarations
class Session;
class ChatRoom;

// ---------------- ChatRoom ----------------

class ChatRoom {
public:
    void join(std::shared_ptr<Session> session);
    void leave(std::shared_ptr<Session> session);
    void broadcast(const std::string& msg);

private:
    std::set<std::shared_ptr<Session>> sessions_;
};

// ---------------- Session ----------------

class Session : public std::enable_shared_from_this<Session> {
public:
    Session(tcp::socket socket, ChatRoom& room);
    void start();
    void deliver(const std::string& msg);
private:
    void do_read();
    void do_write();
    boost::uuids::uuid id() const;
private:
    tcp::socket socket_;
    ChatRoom& room_;
    boost::asio::streambuf buffer_;
    std::deque<std::string> write_queue_;
    boost::uuids::uuid id_;
};

// ---------------- Server ----------------

class Server {
public:
    Server(boost::asio::io_context& io, short port);
private:
    void do_accept();

private:
    tcp::acceptor acceptor_;
    ChatRoom room_;
};

// ---------------- main ----------------
