// include/services/AuthService.h
#pragma once
#include <string>
#include <optional>
#include <nlohmann/json.hpp>
#include "../models/User.h"
#include "../utils/JwtUtils.h"

struct AuthResult {
    bool success = false;
    std::string token;
    User user;
    std::string error_message;
    
    nlohmann::json to_json() const {
        if (success) {
            return {
                {"success", true},
                {"token", token},
                {"user", user.to_json()}
            };
        } else {
            return {
                {"success", false},
                {"error", error_message}
            };
        }
    }
};

class AuthService {
public:
    // Register new user
    static AuthResult register_user(const RegisterRequest& request);
    
    // Login user
    static AuthResult login(const LoginRequest& request);
    
    // Get user by ID
    static std::optional<User> get_user_by_id(int user_id);
    
    // Update user profile
    static bool update_profile(int user_id, const nlohmann::json& updates);
    
    // Change password
    static bool change_password(int user_id, const std::string& old_password, const std::string& new_password);
    
    // Logout (blacklist token) - for future implementation
    static bool logout(const std::string& token);
    
    // Verify token
    static bool verify_token(const std::string& token, JwtPayload& payload);
};
