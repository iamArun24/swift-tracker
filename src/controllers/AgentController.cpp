// src/controllers/AgentController.cpp
#include "../../include/controllers/AgentController.h"
#include "../../include/middleware/AuthMiddleware.h"
#include "../../include/utils/JsonUtils.h"
#include "../../include/database/ShipmentRepository.h"
#include <nlohmann/json.hpp>

crow::response AgentController::get_deliveries(const crow::request& req) {
    auto auth = AuthMiddleware::verify(req);
    if (!auth.success) {
        return AuthMiddleware::unauthorized(auth.error_message);
    }
    
    if (auth.payload.role != "AGENT") {
        return AuthMiddleware::forbidden("Access denied");
    }
    
    int page = 1;
    int limit = 50;
    
    auto url_params = crow::query_string(req.url_params);
    if (url_params.get("page")) {
        page = std::stoi(url_params.get("page"));
    }
    if (url_params.get("limit")) {
        limit = std::stoi(url_params.get("limit"));
    }
    
    int offset = (page - 1) * limit;
    
    auto deliveries = agent_service_.get_agent_deliveries(auth.payload.user_id, limit, offset);
    
    nlohmann::json data = nlohmann::json::array();
    for (const auto& d : deliveries) {
        data.push_back(d.to_json());
    }
    
    int total = ShipmentRepository::get_count_by_agent(auth.payload.user_id);
    nlohmann::json response = JsonUtils::paginated_response(data, total, page, limit);
    return crow::response(200, response.dump());
}

crow::response AgentController::get_delivery(const crow::request& req, int shipment_id) {
    auto auth = AuthMiddleware::verify(req);
    if (!auth.success) {
        return AuthMiddleware::unauthorized(auth.error_message);
    }
    
    if (auth.payload.role != "AGENT") {
        return AuthMiddleware::forbidden("Access denied");
    }
    
    auto delivery = agent_service_.get_delivery_details(auth.payload.user_id, shipment_id);
    
    if (!delivery) {
        nlohmann::json err = JsonUtils::error_response("Delivery not found", 404);
        return crow::response(404, err.dump());
    }
    
    nlohmann::json response = JsonUtils::success_response("", delivery->to_json());
    return crow::response(200, response.dump());
}

crow::response AgentController::update_status(const crow::request& req, int shipment_id) {
    auto auth = AuthMiddleware::verify(req);
    if (!auth.success) {
        return AuthMiddleware::unauthorized(auth.error_message);
    }
    
    if (auth.payload.role != "AGENT") {
        return AuthMiddleware::forbidden("Access denied");
    }
    
    try {
        auto body = nlohmann::json::parse(req.body);
        std::string status = body.value("status", "");
        std::string location = body.value("location", "");
        std::string notes = body.value("notes", "");
        
        if (status.empty() || location.empty()) {
            nlohmann::json err = JsonUtils::error_response("Status and location are required", 400);
            return crow::response(400, err.dump());
        }
        
        if (agent_service_.update_delivery_status(auth.payload.user_id, shipment_id, status, location, notes)) {
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

crow::response AgentController::mark_delivered(const crow::request& req, int shipment_id) {
    auto auth = AuthMiddleware::verify(req);
    if (!auth.success) {
        return AuthMiddleware::unauthorized(auth.error_message);
    }
    
    if (auth.payload.role != "AGENT") {
        return AuthMiddleware::forbidden("Access denied");
    }
    
    try {
        auto body = nlohmann::json::parse(req.body);
        std::string location = body.value("location", "");
        std::string notes = body.value("notes", "");
        std::string recipient_name = body.value("recipient_name", "");
        
        if (location.empty()) {
            nlohmann::json err = JsonUtils::error_response("Location is required", 400);
            return crow::response(400, err.dump());
        }
        
        if (agent_service_.mark_delivered(auth.payload.user_id, shipment_id, location, notes, recipient_name)) {
            nlohmann::json response = JsonUtils::success_response("Delivery marked as completed");
            return crow::response(200, response.dump());
        } else {
            nlohmann::json err = JsonUtils::error_response("Failed to mark as delivered", 400);
            return crow::response(400, err.dump());
        }
        
    } catch (const nlohmann::json::exception& e) {
        nlohmann::json err = JsonUtils::error_response("Invalid JSON", 400);
        return crow::response(400, err.dump());
    }
}

crow::response AgentController::mark_failed(const crow::request& req, int shipment_id) {
    auto auth = AuthMiddleware::verify(req);
    if (!auth.success) {
        return AuthMiddleware::unauthorized(auth.error_message);
    }
    
    if (auth.payload.role != "AGENT") {
        return AuthMiddleware::forbidden("Access denied");
    }
    
    try {
        auto body = nlohmann::json::parse(req.body);
        std::string location = body.value("location", "");
        std::string reason = body.value("reason", "");
        
        if (location.empty() || reason.empty()) {
            nlohmann::json err = JsonUtils::error_response("Location and reason are required", 400);
            return crow::response(400, err.dump());
        }
        
        if (agent_service_.mark_failed(auth.payload.user_id, shipment_id, location, reason)) {
            nlohmann::json response = JsonUtils::success_response("Delivery marked as failed");
            return crow::response(200, response.dump());
        } else {
            nlohmann::json err = JsonUtils::error_response("Failed to mark as failed", 400);
            return crow::response(400, err.dump());
        }
        
    } catch (const nlohmann::json::exception& e) {
        nlohmann::json err = JsonUtils::error_response("Invalid JSON", 400);
        return crow::response(400, err.dump());
    }
}

crow::response AgentController::get_stats(const crow::request& req) {
    auto auth = AuthMiddleware::verify(req);
    if (!auth.success) {
        return AuthMiddleware::unauthorized(auth.error_message);
    }
    
    if (auth.payload.role != "AGENT") {
        return AuthMiddleware::forbidden("Access denied");
    }
    
    auto stats = agent_service_.get_agent_stats(auth.payload.user_id);
    
    nlohmann::json response = JsonUtils::success_response("", stats);
    return crow::response(200, response.dump());
}
