// src/middleware/AuthMiddleware.cpp
#include "../../include/middleware/AuthMiddleware.h"
#include "../../include/utils/JwtUtils.h"
#include <sstream>

AuthCheckResult AuthMiddleware::verify(const crow::request& req) {
    AuthCheckResult result;
    
    // Get Authorization header
    auto auth_header = req.get_header_value("Authorization");
    
    if (auth_header.empty()) {
        result.error_message = "Authorization header missing";
        return result;
    }
    
    // Check if it starts with "Bearer "
    if (auth_header.substr(0, 7) != "Bearer ") {
        result.error_message = "Invalid authorization format";
        return result;
    }
    
    std::string token = auth_header.substr(7); // Remove "Bearer "
    
    JwtPayload payload;
    if (!JwtUtils::verify(token, payload)) {
        result.error_message = "Invalid or expired token";
        return result;
    }
    
    result.success = true;
    result.payload = payload;
    return result;
}

bool AuthMiddleware::has_role(const JwtPayload& payload, const std::string& required_role) {
    return payload.role == required_role;
}

bool AuthMiddleware::has_any_role(const JwtPayload& payload, const std::vector<std::string>& roles) {
    for (const auto& role : roles) {
        if (payload.role == role) {
            return true;
        }
    }
    return false;
}

crow::response AuthMiddleware::unauthorized(const std::string& message) {
    nlohmann::json err = {{"error", message}, {"code", 401}};
    crow::response res(401, err.dump());
    res.set_header("Content-Type", "application/json");
    return res;
}

crow::response AuthMiddleware::forbidden(const std::string& message) {
    nlohmann::json err = {{"error", message}, {"code", 403}};
    crow::response res(403, err.dump());
    res.set_header("Content-Type", "application/json");
    return res;
}

crow::response AuthMiddleware::middleware(const crow::request& req, crow::response& res, 
                                          const std::vector<std::string>& allowed_roles) {
    auto auth = verify(req);
    
    if (!auth.success) {
        return unauthorized(auth.error_message);
    }
    
    if (!allowed_roles.empty() && !has_any_role(auth.payload, allowed_roles)) {
        return forbidden("Insufficient permissions");
    }
    
    // Add user info to request context (would need to extend crow::request in real implementation)
    return std::move(res);
}
