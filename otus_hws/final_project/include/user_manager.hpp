#pragma once

#include <string>
#include <unordered_map>

struct UserData {
    std::string user_name;
    std::string password;
};

class UserManager {
public:
    UserManager();
    bool register_user(const UserData& user);
    void remove_user(const std::string& user_name);
    bool is_registered(const std::string& user_name) const;
    bool authenticate(const UserData& user) const;

private:
    std::unordered_map<std::string, UserData> users_; // maybe some enriched info
};
