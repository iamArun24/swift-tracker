// src/controllers/AnalyticsController.cpp
#include "../../include/controllers/AnalyticsController.h"
#include "../../include/middleware/AuthMiddleware.h"
#include "../../include/utils/JsonUtils.h"
#include <nlohmann/json.hpp>

crow::response AnalyticsController::get_dashboard(const crow::request& req) {
    auto auth = AuthMiddleware::verify(req);
    if (!auth.success) {
        return AuthMiddleware::unauthorized(auth.error_message);
    }
    
    if (auth.payload.role != "ADMIN") {
        return AuthMiddleware::forbidden("Access denied");
    }
    
    auto stats = analytics_service_.get_dashboard_stats();
    
    nlohmann::json response = JsonUtils::success_response("", stats.to_json());
    return crow::response(200, response.dump());
}

crow::response AnalyticsController::get_shipment_trends(const crow::request& req) {
    auto auth = AuthMiddleware::verify(req);
    if (!auth.success) {
        return AuthMiddleware::unauthorized(auth.error_message);
    }
    
    if (auth.payload.role != "ADMIN") {
        return AuthMiddleware::forbidden("Access denied");
    }
    
    std::string period = "daily";
    int days = 30;
    
    auto url_params = crow::query_string(req.url_params);
    if (url_params.get("period")) {
        period = url_params.get("period");
    }
    if (url_params.get("days")) {
        days = std::stoi(url_params.get("days"));
    }
    
    auto trends = analytics_service_.get_shipment_trends(period, days);
    
    nlohmann::json data = nlohmann::json::array();
    for (const auto& t : trends) {
        data.push_back(t.to_json());
    }
    
    nlohmann::json response = JsonUtils::success_response("", data);
    return crow::response(200, response.dump());
}

crow::response AnalyticsController::get_revenue(const crow::request& req) {
    auto auth = AuthMiddleware::verify(req);
    if (!auth.success) {
        return AuthMiddleware::unauthorized(auth.error_message);
    }
    
    if (auth.payload.role != "ADMIN") {
        return AuthMiddleware::forbidden("Access denied");
    }
    
    std::string period = "monthly";
    int months = 12;
    
    auto url_params = crow::query_string(req.url_params);
    if (url_params.get("period")) {
        period = url_params.get("period");
    }
    if (url_params.get("months")) {
        months = std::stoi(url_params.get("months"));
    }
    
    auto revenue = analytics_service_.get_revenue_analytics(period, months);
    
    nlohmann::json response = JsonUtils::success_response("", revenue);
    return crow::response(200, response.dump());
}

crow::response AnalyticsController::get_agent_performance(const crow::request& req) {
    auto auth = AuthMiddleware::verify(req);
    if (!auth.success) {
        return AuthMiddleware::unauthorized(auth.error_message);
    }
    
    if (auth.payload.role != "ADMIN") {
        return AuthMiddleware::forbidden("Access denied");
    }
    
    auto performance = analytics_service_.get_agent_performance();
    
    nlohmann::json data = nlohmann::json::array();
    for (const auto& p : performance) {
        data.push_back(p.to_json());
    }
    
    nlohmann::json response = JsonUtils::success_response("", data);
    return crow::response(200, response.dump());
}

crow::response AnalyticsController::get_popular_routes(const crow::request& req) {
    auto auth = AuthMiddleware::verify(req);
    if (!auth.success) {
        return AuthMiddleware::unauthorized(auth.error_message);
    }
    
    if (auth.payload.role != "ADMIN") {
        return AuthMiddleware::forbidden("Access denied");
    }
    
    int limit = 10;
    
    auto url_params = crow::query_string(req.url_params);
    if (url_params.get("limit")) {
        limit = std::stoi(url_params.get("limit"));
    }
    
    auto routes = analytics_service_.get_popular_routes(limit);
    
    nlohmann::json response = JsonUtils::success_response("", routes);
    return crow::response(200, response.dump());
}
