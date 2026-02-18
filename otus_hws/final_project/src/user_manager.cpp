#include "user_manager.hpp"
#include <stdexcept>

UserManager::UserManager() {

}

bool UserManager::register_user(const UserData& user) {
    if (!is_registered(user.user_name)) {
        const auto [it, inserted] = users_.emplace(user.user_name, user);
        return inserted;
        
    } 
    return false;
}

void UserManager::remove_user(const std::string& user_name) {
    if (is_registered(user_name)) {
        users_.erase(user_name);
    } else {
        throw std::runtime_error("User not registered yet");
    }
}

bool UserManager::is_registered(const std::string& user_name) const {
    return users_.contains(user_name);
}

bool UserManager::authenticate(const UserData& user) const {
    if (is_registered(user.user_name)) {
        const auto& user_data = users_.at(user.user_name);
        return user_data.user_name == user.user_name && user_data.password == user.password;
    }
    return false;
}

