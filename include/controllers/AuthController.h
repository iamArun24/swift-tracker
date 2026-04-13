// include/controllers/AuthController.h
#pragma once
#include <crow.h>
#include "../services/AuthService.h"

class AuthController {
private:
    AuthService auth_service_;
    
public:
    // Register new user
    crow::response register_user(const crow::request& req);
    
    // Login
    crow::response login(const crow::request& req);
    
    // Get current user profile (requires auth)
    crow::response get_profile(const crow::request& req);
    
    // Update profile (requires auth)
    crow::response update_profile(const crow::request& req);
    
    // Change password (requires auth)
    crow::response change_password(const crow::request& req);
};
