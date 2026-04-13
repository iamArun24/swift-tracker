// include/database/UserRepository.h
#pragma once
#include <string>
#include <vector>
#include <optional>
#include <nlohmann/json.hpp>
#include "../models/User.h"

class UserRepository {
public:
    // Create new user
    static bool create(const User& user);
    
    // Find user by ID
    static std::optional<User> find_by_id(int id);
    
    // Find user by email
    static std::optional<User> find_by_email(const std::string& email);
    
    // Find user by UUID
    static std::optional<User> find_by_uuid(const std::string& uuid);
    
    // Update user
    static bool update(const User& user);
    
    // Update last login
    static bool update_last_login(int user_id);
    
    // Update password
    static bool update_password(int user_id, const std::string& new_hash);
    
    // Delete user (soft delete - set is_active = 0)
    static bool deactivate(int user_id);
    
    // Get all users (with optional filters)
    static std::vector<User> get_all(const std::string& role = "", bool active_only = true);
    
    // Check if email exists
    static bool email_exists(const std::string& email);
};
