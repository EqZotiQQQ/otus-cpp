#include "command_parser.hpp"

#include <sstream>

Command CommandParser::parse(const std::string& line) {
    Command cmd;
    cmd.raw = line;
    if (!line.starts_with('/')) {
        return cmd;
    }
    std::istringstream iss(line);
    std::string token;
    iss >> token;
    cmd.type = resolve_command(token);
    while (iss >> token) {
        cmd.args.push_back(token);
    }
    return cmd;
}

CommandType CommandParser::resolve_command(const std::string& cmd) {
    if (cmd == "/register" || cmd == "/reg")
        return CommandType::Register;
    if (cmd == "/login")
        return CommandType::Login;
    if (cmd == "/history" || cmd == "/hist")
        return CommandType::History;
    if (cmd == "/help")
        return CommandType::Help;
    return CommandType::Unknown;
}
