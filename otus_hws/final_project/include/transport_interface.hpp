#pragma once

#include <spdlog/spdlog.h>

#include <boost/asio.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "chat_room.hpp"
#include "schema.pb.h"

using boost::asio::ip::tcp;

class SessionTransportInterface {
public:
    virtual ~SessionTransportInterface() = default;

    virtual void send_protobuf(const chat::ServerMessage& msg) = 0;
    virtual void close() = 0;
    virtual boost::uuids::uuid id() const = 0;
};