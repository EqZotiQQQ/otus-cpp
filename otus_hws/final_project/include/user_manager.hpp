#pragma once

#include <string>
#include <unordered_map>

enum class UserState { ACTIVE = 0, INACTIVE };

struct UserData {
    std::string user_name;
    std::string password;
    UserState state;
};

class UserManager {
public:
    UserManager();
    bool register_user(const std::string& user_name, const std::string& password);
    bool is_registered(const std::string& user_name) const;
    bool authenticate(const UserData& user);
    bool is_logined(const std::string& user_name) const;
    bool log_out(const std::string& user_name);

private:
    std::unordered_map<std::string, UserData> users_;  // maybe some enriched info
};
