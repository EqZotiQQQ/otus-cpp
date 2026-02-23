#include "user_manager.hpp"

#include <spdlog/spdlog.h>

UserManager::UserManager() {
}

bool UserManager::register_user(const std::string& user_name, const std::string& password) {
    if (!is_registered(user_name)) {
        const auto [it, inserted] = users_.emplace(user_name, UserData{user_name, password, UserState::Active});
        spdlog::info("Registered new user: {}", user_name);
        return inserted;
    }
    return false;
}

bool UserManager::is_registered(const std::string& user_name) const {
    return users_.contains(user_name);
}

bool UserManager::is_logined(const std::string& user_name) const {
    return is_registered(user_name) && users_.at(user_name).state == UserState::Active;
}

bool UserManager::authenticate(const UserData& user) {
    if (!is_logined(user.user_name)) {
        auto& user_data = users_[user.user_name];
        bool login_success = user_data.user_name == user.user_name && user_data.password == user.password;
        if (login_success) {
            user_data.state = UserState::Active;
        }
        spdlog::info("User authenticated: {}", user.user_name);
        return login_success;
    }
    return false;
}

bool UserManager::log_out(const std::string& user_name) {
    if (is_logined(user_name)) {
        users_[user_name].state = UserState::Inactive;
        spdlog::info("User log out: {}", user_name);
        return true;
    }
    return false;
}
