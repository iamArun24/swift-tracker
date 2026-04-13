// include/utils/PasswordUtils.h
#pragma once
#include <string>

class PasswordUtils {
public:
    // Hash password using bcrypt
    // Returns hashed password string
    static std::string hash_password(const std::string& password);
    
    // Verify password against hash
    // Returns true if password matches
    static bool verify_password(const std::string& password, const std::string& hash);
    
    // Validate password strength
    // Returns error message if invalid, empty string if valid
    static std::string validate_strength(const std::string& password);
};
