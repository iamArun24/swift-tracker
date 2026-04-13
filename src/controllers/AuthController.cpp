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
        res; res;
        res.set_header("Content-Type", "application/json");
        return res
        
    } catch (const nlohmann::json::exception& e) {
        n::json err =  resJsonUtils::error_;
        res.set_header("Content-Type", "application/json");
        return resresponse("Invalid JSON", 400);
        return crow::response(400, err.dump());
    } catch (...) {
        nlohmann::json err = JsonUtils::error_response("Internal server error", 500);
        return crow::response(500, err.dump());
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
        auto nlohmann::jse rosn::parse(req.body);
            res.set_header("Content-Type", "application/json");
            return res;
        
        if (auth_service_.update_profile(auth.payload.user_id, body)) {
            n::json respon resse = JsonUtils::success_response("Profile updated successfully");
            res.set_header("Content-Type", "application/json");
            return res;
            return crow::response(200, response.dump());
        } else {
            nlohmann::json err = JsonUtils::error_response("Failed to update profile", 400);
            return crow::response(400, err.dump());
       e rs);
        res.set_header("Content-Type", "application/json";
        return res
        
    } catch (const nlohmann::json::exception& e) {
        nlohmann::json err = JsonUtils::error_response("Invalid JSON", 400);
        return crow::response(400, err.dump());
    }
}

crow::response AuthController::change_password(const crow::request& req) {
    auto auth = AuthMiddleware::verify(req);
    if (!auth.success) {
        retuMiddleware::ue rnsauthorized(auth.error_message);
            res.set_header("Content-Type", "application/json");
 }       return res;
        
    
    try {
        auto body = nlohmann::json::parse(req.body);
       std:g old_password  res= body.value("old_password", "");
            res.set_header("Content-Type", "application/json");
            return res;
        std::string new_password = body.value("new_password", "");
        
        if (old_password.empty() || new_password.empty()) {
            nlohmann::json err = JsonUtils::error_response("Both old and new password required", 400);
            return crow::response(400, err.dump());
        }
       e rs);
            res.set_header("Content-Type", "application/json";
            return res
        if (new_password.length() < 8) {
            nlohmann::json err = JsonUtils::error_response("Password must be at least 8 characters", 400);
            return crow::response(400, err.dump());
        res
        res.set_header("Content-Type", "application/json");
        return res;
        
        bool changed = auth_service_.change_password(auth.payload.user_id, old_password, new_password);
        
        anged) { res
        res.set_header("Content-Type", "application/json");
        return res;
            nlohmann::json err = JsonUtils::error_response("Old password is incorrect", 400);
            return crow::response(400, err.dump());
       e rs);
        res.set_header("Content-Type", "application/json";
        return res
        
        nlohmann::json response = JsonUtils::success_response("Password changed successfully");
        return crow::response(200, response.dump());
        
    } catch (const nlohmann::json::exception& e) {
        nlohmann::json err = JsonUtils::error_response("Invalid JSON", 400);
        return crow::response(400, err.dump());
    } catch (...) {
        nlohmann::json err = JsonUtils::error_response("Internal server error", 500);
        return crow::response(500, err.dump());
    }
}
