// include/middleware/AuthMiddleware.h
#pragma once
#include <crow.h>
#include <string>
#include "../utils/JwtUtils.h"

struct AuthCheckResult {
    bool success = false;
    JwtPayload payload;
    std::string error_message;
};

class AuthMiddleware {
public:
    // Verify JWT token from Authorization header
    static AuthCheckResult verify(const crow::request& req);
    
    // Check if user has required role
    static bool has_role(const JwtPayload& payload, const std::string& required_role);
    
    // Check if user has any of the required roles
    static bool has_any_role(const JwtPayload& payload, const std::vector<std::string>& roles);
    
    // Generate unauthorized response
    static crow::response unauthorized(const std::string& message = "Unauthorized");
    
    // Generate forbidden response
    static crow::response forbidden(const std::string& message = "Forbidden");
    
    // Middleware function for Crow
    static struct crow::response middleware(const crow::request& req, crow::response& res, 
                                           const std::vector<std::string>& allowed_roles = {});
};
