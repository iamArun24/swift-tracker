// src/controllers/AuthController.cpp
#include "../../include/controllers/AuthController.h"
#include "../../include/middleware/AuthMiddleware.h"
#include "../../include/utils/JsonUtils.h"
#include <nlohmann/json.hpp>

crow::response AuthController::register_user(const crow::request& req) {
    try {
        auto body = nlohmann::json::parse(req.body);
        auto register_req = RegisterRequest::from_json(body);
        
        auto result = auth_service_.register_user(register_req);
        
        if (!result.success) {
            nlohmann::json err = JsonUtils::error_response(result.error_message, 400);
            crow::response res(400, err.dump());
            res.set_header("Content-Type", "application/json");
            return res;
        }
        
        nlohmann::json response = JsonUtils::success_response("Account created successfully", result.to_json());
        crow::response res(201, response.dump());
        res.set_header("Content-Type", "application/json");
        return res;
        
    } catch (const nlohmann::json::exception& e) {
        nlohmann::json err = JsonUtils::error_response("Invalid JSON", 400);
        crow::response res(400, err.dump());
        res.set_header("Content-Type", "application/json");
        return res;
    } catch (...) {
        nlohmann::json err = JsonUtils::error_response("Internal server error", 500);
        crow::response res(500, err.dump());
        res.set_header("Content-Type", "application/json");
        return res;
    }
}

crow::response AuthController::login(const crow::request& req) {
    try {
        auto body = nlohmann::json::parse(req.body);
        auto login_req = LoginRequest::from_json(body);
        
        auto result = auth_service_.login(login_req);
        
        if (!result.success) {
            nlohmann::json err = JsonUtils::error_response(result.error_message, 401);
            crow::response res(401, err.dump());
            res.set_header("Content-Type", "application/json");
            return res;
        }
        
        nlohmann::json response = JsonUtils::success_response("Login successful", result.to_json());
        crow::response res(200, response.dump());
        res.set_header("Content-Type", "application/json");
        return res;
        
    } catch (const nlohmann::json::exception& e) {
        nlohmann::json err = JsonUtils::error_response("Invalid JSON", 400);
        crow::response res(400, err.dump());
        res.set_header("Content-Type", "application/json");
        return res;
    } catch (...) {
        nlohmann::json err = JsonUtils::error_response("Internal server error", 500);
        crow::response res(500, err.dump());
        res.set_header("Content-Type", "application/json");
        return res;
    }
}

crow::response AuthController::get_profile(const crow::request& req) {
    auto auth = AuthMiddleware::verify(req);
    if (!auth.success) {
        return AuthMiddleware::unauthorized(auth.error_message);
    }
    
    auto user = auth_service_.get_user_by_id(auth.payload.user_id);
    if (!user) {
        nlohmann::json err = JsonUtils::error_response("User not found", 404);
        crow::response res(404, err.dump());
        res.set_header("Content-Type", "application/json");
        return res;
    }
    
    nlohmann::json response = JsonUtils::success_response("", user->to_json());
    crow::response res(200, response.dump());
    res.set_header("Content-Type", "application/json");
    return res;
}

crow::response AuthController::update_profile(const crow::request& req) {
    auto auth = AuthMiddleware::verify(req);
    if (!auth.success) {
        return AuthMiddleware::unauthorized(auth.error_message);
    }
    
    try {
        auto body = nlohmann::json::parse(req.body);
        
        if (auth_service_.update_profile(auth.payload.user_id, body)) {
            nlohmann::json response = JsonUtils::success_response("Profile updated successfully");
            crow::response res(200, response.dump());
            res.set_header("Content-Type", "application/json");
            return res;
        } else {
            nlohmann::json err = JsonUtils::error_response("Failed to update profile", 400);
            crow::response res(400, err.dump());
            res.set_header("Content-Type", "application/json");
            return res;
        }
        
    } catch (const nlohmann::json::exception& e) {
        nlohmann::json err = JsonUtils::error_response("Invalid JSON", 400);
        crow::response res(400, err.dump());
        res.set_header("Content-Type", "application/json");
        return res;
    }
}

crow::response AuthController::change_password(const crow::request& req) {
    auto auth = AuthMiddleware::verify(req);
    if (!auth.success) {
        return AuthMiddleware::unauthorized(auth.error_message);
    }
    
    try {
        auto body = nlohmann::json::parse(req.body);
        std::string old_password = body.value("old_password", "");
        std::string new_password = body.value("new_password", "");
        
        if (old_password.empty() || new_password.empty()) {
            nlohmann::json err = JsonUtils::error_response("Both old and new password required", 400);
            crow::response res(400, err.dump());
            res.set_header("Content-Type", "application/json");
            return res;
        }
        
        if (new_password.length() < 8) {
            nlohmann::json err = JsonUtils::error_response("Password must be at least 8 characters", 400);
            crow::response res(400, err.dump());
            res.set_header("Content-Type", "application/json");
            return res;
        }
        
        bool changed = auth_service_.change_password(auth.payload.user_id, old_password, new_password);
        
        if (!changed) {
            nlohmann::json err = JsonUtils::error_response("Old password is incorrect", 400);
            crow::response res(400, err.dump());
            res.set_header("Content-Type", "application/json");
            return res;
        }
        
        nlohmann::json response = JsonUtils::success_response("Password changed successfully");
        crow::response res(200, response.dump());
        res.set_header("Content-Type", "application/json");
        return res;
        
    } catch (const nlohmann::json::exception& e) {
        nlohmann::json err = JsonUtils::error_response("Invalid JSON", 400);
        crow::response res(400, err.dump());
        res.set_header("Content-Type", "application/json");
        return res;
    } catch (...) {
        nlohmann::json err = JsonUtils::error_response("Internal server error", 500);
        crow::response res(500, err.dump());
        res.set_header("Content-Type", "application/json");
        return res;
    }
}
