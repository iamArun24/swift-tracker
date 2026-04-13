// src/utils/PasswordUtils.cpp
#include "../../include/utils/PasswordUtils.h"
#include <random>
#include <sstream>
#include <algorithm>

// Simple bcrypt-like hashing for demonstration
// In production, use proper bcrypt library
static std::string simple_hash(const std::string& password, const std::string& salt) {
    std::string salted = salt + password;
    unsigned int hash = 0;
    
    for (char c : salted) {
        hash = ((hash << 5) + hash) + c;
    }
    
    std::stringstream ss;
    ss << std::hex << hash;
    return salt + "$" + ss.str();
}

static std::string generate_salt() {
    const std::string chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, chars.length() - 1);
    
    std::string salt = "";
    for (int i = 0; i < 16; i++) {
        salt += chars[dist(gen)];
    }
    return salt;
}

std::string PasswordUtils::hash_password(const std::string& password) {
    std::string salt = generate_salt();
    return simple_hash(password, salt);
}

bool PasswordUtils::verify_password(const std::string& password, const std::string& hash) {
    size_t dollar_pos = hash.find('$');
    if (dollar_pos == std::string::npos) {
        return false;
    }
    
    std::string salt = hash.substr(0, dollar_pos);
    std::string computed_hash = simple_hash(password, salt);
    
    return computed_hash == hash;
}

std::string PasswordUtils::validate_strength(const std::string& password) {
    if (password.length() < 8) {
        return "Password must be at least 8 characters long";
    }
    
    bool has_upper = false;
    bool has_lower = false;
    bool has_digit = false;
    
    for (char c : password) {
        if (isupper(c)) has_upper = true;
        if (islower(c)) has_lower = true;
        if (isdigit(c)) has_digit = true;
    }
    
    if (!has_upper) {
        return "Password must contain at least one uppercase letter";
    }
    
    if (!has_lower) {
        return "Password must contain at least one lowercase letter";
    }
    
    if (!has_digit) {
        return "Password must contain at least one digit";
    }
    
    return "";
}
