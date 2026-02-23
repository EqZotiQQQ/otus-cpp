#pragma once

#include <string>
#include <unordered_map>

enum class UserState { Active = 0, Inactive };

struct UserData {
    std::string user_name;
    std::string password;
    UserState state;
};

class UserManager {
public:
    UserManager();
    [[nodiscard]] bool register_user(const std::string& user_name, const std::string& password);
    [[nodiscard]] bool is_registered(const std::string& user_name) const;
    [[nodiscard]] bool authenticate(const UserData& user);
    [[nodiscard]] bool is_logined(const std::string& user_name) const;
    bool log_out(const std::string& user_name);

private:
    std::unordered_map<std::string, UserData> users_;  // maybe some enriched info
};
