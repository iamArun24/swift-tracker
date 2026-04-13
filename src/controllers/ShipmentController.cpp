// src/controllers/ShipmentController.cpp
#include "../../include/controllers/ShipmentController.h"
#include "../../include/middleware/AuthMiddleware.h"
#include "../../include/utils/JsonUtils.h"
#include "../../include/database/ShipmentRepository.h"
#include "../../include/services/TrackingService.h"
#include <nlohmann/json.hpp>

crow::response ShipmentController::create_shipment(const crow::request& req) {
    auto auth = AuthMiddleware::verify(req);
    if (!auth.success) {
        return AuthMiddleware::unauthorized(auth.error_message);
    }
    
    // Check if user has appropriate role
    if (!AuthMiddleware::has_any_role(auth.payload, {"SENDER", "ADMIN"})) {
        return AuthMiddleware::forbidden("Insufficient permissions");
    }
    
    try {
        auto body = nlohmann::json::parse(req.body);
        auto create_req = CreateShipmentRequest::from_json(body);
        
        // Calculate distance (simplified - in real app would use actual distance calculation)
        double distance = 500.0; // Default distance for demo
        
        auto shipment = shipment_service_.create_shipment(auth.payload.user_id, create_req);
        
        if (!shipment) {
            nlohmann::json err = JsonUtils::error_response("Failed to create shipment", 500);
            return crow::response(500, err.dump());
        }
        
        nlohmann::json response = JsonUtils::success_response("Shipment created successfully", shipment->to_json());
        return crow::response(201, response.dump());
        
    } catch (const nlohmann::json::exception& e) {
        nlohmann::json err = JsonUtils::error_response("Invalid JSON", 400);
        return crow::response(400, err.dump());
    } catch (...) {
        nlohmann::json err = JsonUtils::error_response("Internal server error", 500);
        return crow::response(500, err.dump());
    }
}

crow::response ShipmentController::get_shipments(const crow::request& req) {
    auto auth = AuthMiddleware::verify(req);
    if (!auth.success) {
        return AuthMiddleware::unauthorized(auth.error_message);
    }
    
    // Parse query parameters
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
    
    auto shipments = shipment_service_.get_sender_shipments(auth.payload.user_id, limit, offset);
    
    nlohmann::json data = nlohmann::json::array();
    for (const auto& s : shipments) {
        data.push_back(s.to_json());
    }
    
    int total = ShipmentRepository::get_count_by_sender(auth.payload.user_id);
    nlohmann::json response = JsonUtils::paginated_response(data, total, page, limit);
    return crow::response(200, response.dump());
}

crow::response ShipmentController::get_shipment(const crow::request& req, int shipment_id) {
    auto auth = AuthMiddleware::verify(req);
    if (!auth.success) {
        return AuthMiddleware::unauthorized(auth.error_message);
    }
    
    auto shipment = shipment_service_.get_shipment(shipment_id);
    
    if (!shipment) {
        nlohmann::json err = JsonUtils::error_response("Shipment not found", 404);
        return crow::response(404, err.dump());
    }
    
    // Check if user owns this shipment or is admin
    if (shipment->sender_id != auth.payload.user_id && auth.payload.role != "ADMIN") {
        return AuthMiddleware::forbidden("Access denied");
    }
    
    nlohmann::json response = JsonUtils::success_response("", shipment->to_json());
    return crow::response(200, response.dump());
}

crow::response ShipmentController::get_tracking(const crow::request& req, int shipment_id) {
    auto auth = AuthMiddleware::verify(req);
    if (!auth.success) {
        return AuthMiddleware::unauthorized(auth.error_message);
    }
    
    auto shipment = shipment_service_.get_shipment(shipment_id);
    
    if (!shipment) {
        nlohmann::json err = JsonUtils::error_response("Shipment not found", 404);
        return crow::response(404, err.dump());
    }
    
    // Check if user owns this shipment or is admin
    if (shipment->sender_id != auth.payload.user_id && auth.payload.role != "ADMIN") {
        return AuthMiddleware::forbidden("Access denied");
    }
    
    auto history = TrackingService::get_history(shipment_id);
    
    nlohmann::json data = nlohmann::json::array();
    for (const auto& event : history) {
        data.push_back(event.to_json());
    }
    
    nlohmann::json response = JsonUtils::success_response("", data);
    return crow::response(200, response.dump());
}

crow::response ShipmentController::price_estimate(const crow::request& req) {
    auto auth = AuthMiddleware::verify(req);
    if (!auth.success) {
        return AuthMiddleware::unauthorized(auth.error_message);
    }
    
    try {
        auto body = nlohmann::json::parse(req.body);
        auto create_req = CreateShipmentRequest::from_json(body);
        
        double distance = body.value("distance_km", 500.0);
        
        auto price = shipment_service_.calculate_price_estimate(create_req, distance);
        
        nlohmann::json response = JsonUtils::success_response("", price.to_json());
        return crow::response(200, response.dump());
        
    } catch (const nlohmann::json::exception& e) {
        nlohmann::json err = JsonUtils::error_response("Invalid JSON", 400);
        return crow::response(400, err.dump());
    }
}

crow::response ShipmentController::cancel_shipment(const crow::request& req, int shipment_id) {
    auto auth = AuthMiddleware::verify(req);
    if (!auth.success) {
        return AuthMiddleware::unauthorized(auth.error_message);
    }
    
    auto shipment = shipment_service_.get_shipment(shipment_id);
    
    if (!shipment) {
        nlohmann::json err = JsonUtils::error_response("Shipment not found", 404);
        return crow::response(404, err.dump());
    }
    
    // Check if user owns this shipment or is admin
    if (shipment->sender_id != auth.payload.user_id && auth.payload.role != "ADMIN") {
        return AuthMiddleware::forbidden("Access denied");
    }
    
    if (shipment_service_.cancel_shipment(shipment_id)) {
        nlohmann::json response = JsonUtils::success_response("Shipment cancelled successfully");
        return crow::response(200, response.dump());
    } else {
        nlohmann::json err = JsonUtils::error_response("Cannot cancel shipment (not in BOOKED status)", 400);
        return crow::response(400, err.dump());
    }
}
