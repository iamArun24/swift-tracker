// src/services/AuthService.cpp
#include "../../include/services/AuthService.h"
#include "../../include/database/UserRepository.h"
#include "../../include/utils/PasswordUtils.h"
#include "../../include/utils/JwtUtils.h"
#include "../../include/utils/ValidationUtils.h"
#include "../../include/utils/DateTimeUtils.h"
#include "../../include/config/Config.h"

AuthResult AuthService::register_user(const RegisterRequest& request) {
    AuthResult result;
    
    // Validate input
    if (request.full_name.empty() || request.email.empty() || request.password.empty()) {
        result.error_message = "Name, email, and password are required";
        return result;
    }
    
    if (!ValidationUtils::is_valid_email(request.email)) {
        result.error_message = "Invalid email format";
        return result;
    }
    
    std::string password_error = PasswordUtils::validate_strength(request.password);
    if (!password_error.empty()) {
        result.error_message = password_error;
        return result;
    }
    
    // Check if email already exists
    if (UserRepository::email_exists(request.email)) {
        result.error_message = "Email already registered";
        return result;
    }
    
    // Create user
    User user;
    user.uuid = DateTimeUtils::generate_uuid();
    user.full_name = request.full_name;
    user.email = request.email;
    user.phone = request.phone;
    user.password_hash = PasswordUtils::hash_password(request.password);
    user.role = UserRole::SENDER;
    user.is_active = true;
    user.city = request.city;
    user.state = request.state;
    user.pincode = request.pincode;
    user.address = request.address;
    user.created_at = DateTimeUtils::now();
    user.updated_at = DateTimeUtils::now();
    
    if (!UserRepository::create(user)) {
        result.error_message = "Failed to create user";
        return result;
    }
    
    // Get the created user with ID
    auto created_user = UserRepository::find_by_email(request.email);
    if (!created_user) {
        result.error_message = "Failed to retrieve created user";
        return result;
    }
    
    // Generate JWT token
    JwtPayload payload;
    payload.user_id = created_user->id;
    payload.email = created_user->email;
    payload.role = role_to_string(created_user->role);
    payload.exp = JwtUtils::get_expiry(Config::JWT_EXPIRY_HOURS);
    
    result.success = true;
    result.token = JwtUtils::generate(payload);
    result.user = *created_user;
    
    return result;
}

AuthResult AuthService::login(const LoginRequest& request) {
    AuthResult result;
    
    if (request.email.empty() || request.password.empty()) {
        result.error_message = "Email and password are required";
        return result;
    }
    
    // Find user by email
    auto user = UserRepository::find_by_email(request.email);
    if (!user) {
        result.error_message = "Invalid email or password";
        return result;
    }
    
    // Check if user is active
    if (!user->is_active) {
        result.error_message = "Account is deactivated";
        return result;
    }
    
    // Verify password
    if (!PasswordUtils::verify_password(request.password, user->password_hash)) {
        result.error_message = "Invalid email or password";
        return result;
    }
    
    // Update last login
    UserRepository::update_last_login(user->id);
    
    // Generate JWT token
    JwtPayload payload;
    payload.user_id = user->id;
    payload.email = user->email;
    payload.role = role_to_string(user->role);
    payload.exp = JwtUtils::get_expiry(Config::JWT_EXPIRY_HOURS);
    
    result.success = true;
    result.token = JwtUtils::generate(payload);
    result.user = *user;
    
    return result;
}

std::optional<User> AuthService::get_user_by_id(int user_id) {
    return UserRepository::find_by_id(user_id);
}

bool AuthService::update_profile(int user_id, const nlohmann::json& updates) {
    auto user = UserRepository::find_by_id(user_id);
    if (!user) {
        return false;
    }
    
    if (updates.contains("full_name")) {
        user->full_name = updates["full_name"];
    }
    if (updates.contains("phone")) {
        user->phone = updates["phone"];
    }
    if (updates.contains("city")) {
        user->city = updates["city"];
    }
    if (updates.contains("state")) {
        user->state = updates["state"];
    }
    if (updates.contains("pincode")) {
        user->pincode = updates["pincode"];
    }
    if (updates.contains("address")) {
        user->address = updates["address"];
    }
    
    return UserRepository::update(*user);
}

bool AuthService::change_password(int user_id, const std::string& old_password, const std::string& new_password) {
    auto user = UserRepository::find_by_id(user_id);
    if (!user) {
        return false;
    }
    
    // Verify old password
    if (!PasswordUtils::verify_password(old_password, user->password_hash)) {
        return false;
    }
    
    // Validate new password
    std::string error = PasswordUtils::validate_strength(new_password);
    if (!error.empty()) {
        return false;
    }
    
    // Hash new password
    std::string new_hash = PasswordUtils::hash_password(new_password);
    
    return UserRepository::update_password(user_id, new_hash);
}

bool AuthService::logout(const std::string& token) {
    // For now, just return true
    // In a full implementation, you would add the token to a blacklist
    return true;
}

bool AuthService::verify_token(const std::string& token, JwtPayload& payload) {
    return JwtUtils::verify(token, payload);
}
