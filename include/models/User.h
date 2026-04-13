// include/models/User.h
#pragma once
#include <string>
#include <optional>
#include <nlohmann/json.hpp>

enum class UserRole {
    ADMIN,
    SENDER, 
    AGENT,
    PUBLIC
};

// Convert enum to string
inline std::string role_to_string(UserRole role) {
    switch(role) {
        case UserRole::ADMIN:   return "ADMIN";
        case UserRole::SENDER:  return "SENDER";
        case UserRole::AGENT:   return "AGENT";
        case UserRole::PUBLIC:  return "PUBLIC";
        default:                return "SENDER";
    }
}

// Convert string to enum
inline UserRole string_to_role(const std::string& role_str) {
    if (role_str == "ADMIN")  return UserRole::ADMIN;
    if (role_str == "AGENT")  return UserRole::AGENT;
    if (role_str == "PUBLIC") return UserRole::PUBLIC;
    return UserRole::SENDER;
}

struct User {
    int id = 0;
    std::string uuid;
    std::string full_name;
    std::string email;
    std::string phone;
    std::string password_hash;
    UserRole role = UserRole::SENDER;
    bool is_active = true;
    std::string city;
    std::string state;
    std::string pincode;
    std::string address;
    std::string created_at;
    std::string updated_at;
    std::string last_login;
    
    // Convert to JSON (safe version - no password hash)
    nlohmann::json to_json() const {
        return {
            {"id",          id},
            {"uuid",        uuid},
            {"full_name",   full_name},
            {"email",       email},
            {"phone",       phone},
            {"role",        role_to_string(role)},
            {"is_active",   is_active},
            {"city",        city},
            {"state",       state},
            {"pincode",     pincode},
            {"address",     address},
            {"created_at",  created_at},
            {"last_login",  last_login}
        };
    }
};

// DTO for user registration
struct RegisterRequest {
    std::string full_name;
    std::string email;
    std::string phone;
    std::string password;
    std::string city;
    std::string state;
    std::string pincode;
    std::string address;
    
    // Parse from JSON
    static RegisterRequest from_json(const nlohmann::json& j) {
        RegisterRequest req;
        req.full_name = j.value("full_name", "");
        req.email     = j.value("email", "");
        req.phone     = j.value("phone", "");
        req.password  = j.value("password", "");
        req.city      = j.value("city", "");
        req.state     = j.value("state", "");
        req.pincode   = j.value("pincode", "");
        req.address   = j.value("address", "");
        return req;
    }
};

// DTO for login
struct LoginRequest {
    std::string email;
    std::string password;
    
    static LoginRequest from_json(const nlohmann::json& j) {
        LoginRequest req;
        req.email    = j.value("email", "");
        req.password = j.value("password", "");
        return req;
    }
};

// Auth response DTO
struct AuthResponse {
    std::string token;
    User user;
    
    nlohmann::json to_json() const {
        return {
            {"token", token},
            {"user",  user.to_json()}
        };
    }
};
