#include <string>
#include <vector>

enum class CommandType { Register, Login, History, Users, Help, Unknown };

struct Command {
    CommandType type = CommandType::Unknown;
    std::vector<std::string> args;
    std::string raw;
};

class CommandParser {
public:
    static Command parse(const std::string& line);

private:
    static CommandType resolve_command(const std::string& cmd);
};
