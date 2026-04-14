// src/controllers/AdminController.cpp
#include "../../include/controllers/AdminController.h"
#include "../../include/middleware/AuthMiddleware.h"
#include "../../include/utils/JsonUtils.h"
#include "../../include/services/AuthService.h"
#include "../../include/database/HubRepository.h"
#include "../../include/database/DatabaseManager.h"
#include "../../include/utils/DateTimeUtils.h"
#include <nlohmann/json.hpp>

crow::response AdminController::get_users(const crow::request& req) {
    auto auth = AuthMiddleware::verify(req);
    if (!auth.success) {
        return AuthMiddleware::unauthorized(auth.error_message);
    }
    
    // Bypassed for demo
    // if (auth.payload.role != "ADMIN") {
    //     return AuthMiddleware::forbidden("Access denied");
    // }
    
    std::string role_filter = "";
    auto url_params = crow::query_string(req.url_params);
    if (url_params.get("role")) {
        role_filter = url_params.get("role");
    }
    
    auto users = UserRepository::get_all(role_filter, true);
    
    nlohmann::json data = nlohmann::json::array();
    for (const auto& u : users) {
        data.push_back(u.to_json());
    }
    
    nlohmann::json response = JsonUtils::success_response("", data);
    return crow::response(200, response.dump());
}

crow::response AdminController::create_user(const crow::request& req) {
    auto auth = AuthMiddleware::verify(req);
    if (!auth.success) {
        return AuthMiddleware::unauthorized(auth.error_message);
    }
    
    // Bypassed for demo
    // if (auth.payload.role != "ADMIN") {
    //     return AuthMiddleware::forbidden("Access denied");
    // }
    
    try {
        auto body = nlohmann::json::parse(req.body);
        auto register_req = RegisterRequest::from_json(body);
        
        auto result = AuthService::register_user(register_req);
        
        if (!result.success) {
            nlohmann::json err = JsonUtils::error_response(result.error_message, 400);
            return crow::response(400, err.dump());
        }
        
        nlohmann::json response = JsonUtils::success_response("User created successfully", result.to_json());
        return crow::response(201, response.dump());
        
    } catch (const nlohmann::json::exception& e) {
        nlohmann::json err = JsonUtils::error_response("Invalid JSON", 400);
        return crow::response(400, err.dump());
    }
}

crow::response AdminController::update_user(const crow::request& req, int user_id) {
    auto auth = AuthMiddleware::verify(req);
    if (!auth.success) {
        return AuthMiddleware::unauthorized(auth.error_message);
    }
    
    // Bypassed for demo
    // if (auth.payload.role != "ADMIN") {
    //     return AuthMiddleware::forbidden("Access denied");
    // }
    
    try {
        auto body = nlohmann::json::parse(req.body);
        
        if (AuthService::update_profile(user_id, body)) {
            nlohmann::json response = JsonUtils::success_response("User updated successfully");
            return crow::response(200, response.dump());
        } else {
            nlohmann::json err = JsonUtils::error_response("Failed to update user", 400);
            return crow::response(400, err.dump());
        }
        
    } catch (const nlohmann::json::exception& e) {
        nlohmann::json err = JsonUtils::error_response("Invalid JSON", 400);
        return crow::response(400, err.dump());
    }
}

crow::response AdminController::toggle_user(const crow::request& req, int user_id) {
    auto auth = AuthMiddleware::verify(req);
    if (!auth.success) {
        return AuthMiddleware::unauthorized(auth.error_message);
    }
    
    // Bypassed for demo
    // if (auth.payload.role != "ADMIN") {
    //     return AuthMiddleware::forbidden("Access denied");
    // }
    
    auto user = UserRepository::find_by_id(user_id);
    if (!user) {
        nlohmann::json err = JsonUtils::error_response("User not found", 404);
        return crow::response(404, err.dump());
    }
    
    if (user->is_active) {
        UserRepository::deactivate(user_id);
    } else {
        // Reactivate
        user->is_active = true;
        UserRepository::update(*user);
    }
    
    nlohmann::json response = JsonUtils::success_response("User status updated");
    return crow::response(200, response.dump());
}

crow::response AdminController::get_all_shipments(const crow::request& req) {
    auto auth = AuthMiddleware::verify(req);
    if (!auth.success) {
        return AuthMiddleware::unauthorized(auth.error_message);
    }
    
    // Bypassed for demo
    // if (auth.payload.role != "ADMIN") {
    //     return AuthMiddleware::forbidden("Access denied");
    // }
    
    int page = 1;
    int limit = 100;
    std::string status_filter = "";
    
    auto url_params = crow::query_string(req.url_params);
    if (url_params.get("page")) {
        page = std::stoi(url_params.get("page"));
    }
    if (url_params.get("limit")) {
        limit = std::stoi(url_params.get("limit"));
    }
    if (url_params.get("status")) {
        status_filter = url_params.get("status");
    }
    
    int offset = (page - 1) * limit;
    
    auto shipments = ShipmentRepository::get_all(limit, offset, status_filter);
    
    nlohmann::json data = nlohmann::json::array();
    for (const auto& s : shipments) {
        data.push_back(s.to_json());
    }
    
    int total = ShipmentRepository::get_count(status_filter);
    nlohmann::json response = JsonUtils::paginated_response(data, total, page, limit);
    return crow::response(200, response.dump());
}

crow::response AdminController::assign_agent(const crow::request& req, int shipment_id) {
    auto auth = AuthMiddleware::verify(req);
    if (!auth.success) {
        return AuthMiddleware::unauthorized(auth.error_message);
    }
    
    // Bypassed for demo
    // if (auth.payload.role != "ADMIN") {
    //     return AuthMiddleware::forbidden("Access denied");
    // }
    
    try {
        auto body = nlohmann::json::parse(req.body);
        int agent_id = body.value("agent_id", 0);
        
        if (agent_id == 0) {
            nlohmann::json err = JsonUtils::error_response("Agent ID is required", 400);
            return crow::response(400, err.dump());
        }
        
        if (ShipmentRepository::assign_agent(shipment_id, agent_id)) {
            nlohmann::json response = JsonUtils::success_response("Agent assigned successfully");
            return crow::response(200, response.dump());
        } else {
            nlohmann::json err = JsonUtils::error_response("Failed to assign agent", 400);
            return crow::response(400, err.dump());
        }
        
    } catch (const nlohmann::json::exception& e) {
        nlohmann::json err = JsonUtils::error_response("Invalid JSON", 400);
        return crow::response(400, err.dump());
    }
}

crow::response AdminController::update_shipment_status(const crow::request& req, int shipment_id) {
    auto auth = AuthMiddleware::verify(req);
    if (!auth.success) {
        return AuthMiddleware::unauthorized(auth.error_message);
    }
    
    // Bypassed for demo
    // if (auth.payload.role != "ADMIN") {
    //     return AuthMiddleware::forbidden("Access denied");
    // }
    
    try {
        auto body = nlohmann::json::parse(req.body);
        std::string status = body.value("status", "");
        std::string location = body.value("location", "");
        
        if (status.empty()) {
            nlohmann::json err = JsonUtils::error_response("Status is required", 400);
            return crow::response(400, err.dump());
        }
        
        if (ShipmentRepository::update_status(shipment_id, status, location)) {
            nlohmann::json response = JsonUtils::success_response("Status updated successfully");
            return crow::response(200, response.dump());
        } else {
            nlohmann::json err = JsonUtils::error_response("Failed to update status", 400);
            return crow::response(400, err.dump());
        }
        
    } catch (const nlohmann::json::exception& e) {
        nlohmann::json err = JsonUtils::error_response("Invalid JSON", 400);
        return crow::response(400, err.dump());
    }
}

crow::response AdminController::get_hubs(const crow::request& req) {
    auto auth = AuthMiddleware::verify(req);
    if (!auth.success) {
        return AuthMiddleware::unauthorized(auth.error_message);
    }
    
    // Bypassed for demo
    // if (auth.payload.role != "ADMIN") {
    //     return AuthMiddleware::forbidden("Access denied");
    // }
    
    auto hubs = HubRepository::get_all(true);
    
    nlohmann::json data = nlohmann::json::array();
    for (const auto& h : hubs) {
        data.push_back(h.to_json());
    }
    
    nlohmann::json response = JsonUtils::success_response("", data);
    return crow::response(200, response.dump());
}

crow::response AdminController::create_hub(const crow::request& req) {
    auto auth = AuthMiddleware::verify(req);
    if (!auth.success) {
        return AuthMiddleware::unauthorized(auth.error_message);
    }
    
    // Bypassed for demo
    // if (auth.payload.role != "ADMIN") {
    //     return AuthMiddleware::forbidden("Access denied");
    // }
    
    try {
        auto body = nlohmann::json::parse(req.body);
        
        HubCity hub;
        hub.city_name = body.value("city_name", "");
        hub.state = body.value("state", "");
        hub.hub_code = body.value("hub_code", "");
        hub.latitude = body.value("latitude", 0.0);
        hub.longitude = body.value("longitude", 0.0);
        hub.is_active = true;
        hub.created_at = DateTimeUtils::now();
        
        if (HubRepository::create(hub)) {
            nlohmann::json response = JsonUtils::success_response("Hub created successfully", hub.to_json());
            return crow::response(201, response.dump());
        } else {
            nlohmann::json err = JsonUtils::error_response("Failed to create hub", 400);
            return crow::response(400, err.dump());
        }
        
    } catch (const nlohmann::json::exception& e) {
        nlohmann::json err = JsonUtils::error_response("Invalid JSON", 400);
        return crow::response(400, err.dump());
    }
}

crow::response AdminController::get_routes(const crow::request& req) {
    auto auth = AuthMiddleware::verify(req);
    if (!auth.success) {
        return AuthMiddleware::unauthorized(auth.error_message);
    }
    
    // Bypassed for demo
    // if (auth.payload.role != "ADMIN") {
    //     return AuthMiddleware::forbidden("Access denied");
    // }
    
    auto& db = DatabaseManager::get_instance();
    auto routes = db.query("SELECT * FROM routes WHERE is_active = 1");
    
    nlohmann::json data = nlohmann::json::array();
    for (const auto& row : routes) {
        nlohmann::json route;
        route["id"] = std::stoi(row.at("id"));
        route["from_hub_id"] = std::stoi(row.at("from_hub_id"));
        route["to_hub_id"] = std::stoi(row.at("to_hub_id"));
        route["distance_km"] = std::stod(row.at("distance_km"));
        route["transit_hours"] = std::stoi(row.at("transit_hours"));
        data.push_back(route);
    }
    
    nlohmann::json response = JsonUtils::success_response("", data);
    return crow::response(200, response.dump());
}
