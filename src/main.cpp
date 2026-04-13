// src/main.cpp
#include <crow.h>
#include "../include/database/DatabaseManager.h"
#include "../include/utils/JwtUtils.h"
#include "../include/config/Config.h"
#include "../include/services/RouteEngine.h"
#include "../include/middleware/CorsMiddleware.h"
#include "../include/middleware/LoggerMiddleware.h"
#include "../include/controllers/AuthController.h"
#include "../include/controllers/ShipmentController.h"
#include "../include/controllers/TrackingController.h"
#include "../include/controllers/AgentController.h"
#include "../include/controllers/AdminController.h"
#include "../include/controllers/AnalyticsController.h"
#include <iostream>

int main() {
    crow::SimpleApp app;
    
    // Log initialization
    std::cout << "========================================\n";
    std::cout << "  SwiftTrack Courier Tracking System  \n";
    std::cout << "  Version: " << Config::APP_VERSION << "\n";
    std::cout << "========================================\n";
    
    // Initialize Database
    try {
        DatabaseManager::initialize(Config::get_db_path());
        std::cout << "[DatabaseManager] Database initialized successfully\n";
    } catch (const std::exception& e) {
        std::cerr << "[DatabaseManager] Error: " << e.what() << std::endl;
        return 1;
    }
    
    int port = Config::get_port();
    std::cout << "\nStarting server on port " << port << "...\n";
    std::cout << "Health check: http://localhost:" << port << "/api/health\n";
    std::cout << "API Base: http://localhost:" << port << "/api\n\n";
    
    // Initialize JWT secret
    JwtUtils::set_secret(Config::get_jwt_secret());
    
    // Initialize route engine
    RouteEngine::initialize();
    
    // Apply CORS middleware globally
    app.loglevel(crow::LogLevel::Info);
    
    // Controllers
    AuthController auth_controller;
    ShipmentController shipment_controller;
    TrackingController tracking_controller;
    AgentController agent_controller;
    AdminController admin_controller;
    AnalyticsController analytics_controller;
    
    // ============================================================
    // GLOBAL CORS MIDDLEWARE (Handle OPTIONS preflight)
    // ============================================================
    app.loglevel(crow::LogLevel::Warning);
    
    CROW_ROUTE(app, "/")
    .methods("OPTIONS"_method)
    ([](const crow::request& req){
        return CorsMiddleware::handle_options();
    });
    
    CROW_ROUTE(app, "/<path>")
    .methods("OPTIONS"_method)
    ([](const crow::request& req, std::string path){
        return CorsMiddleware::handle_options();
    });
    
    // ============================================================
    // HEALTH CHECK
    // ============================================================
    CROW_ROUTE(app, "/api/health")
    ([](){
        nlohmann::json response = {
            {"status", "healthy"},
            {"app", Config::APP_NAME},
            {"version", Config::APP_VERSION},
            {"timestamp", std::time(nullptr)}
        };
        crow::response res(200, response.dump());
        res.set_header("Content-Type", "application/json");
        CorsMiddleware::add_headers(res);
        return res;
    });
    
    // ============================================================
    // PUBLIC TRACKING (No auth required)
    // ============================================================
    CROW_ROUTE(app, "/api/track/<string>")
    ([&tracking_controller](const crow::request& req, std::string tracking_number){
        auto res = tracking_controller.track_public(req, tracking_number);
        CorsMiddleware::add_headers(res);
        return res;
    });
    
    CROW_ROUTE(app, "/api/hubs/public")
    ([](){
        auto hubs = RouteEngine::get_all_hubs_json();
        nlohmann::json response = {
            {"data", hubs}
        };
        crow::response res(200, response.dump());
        res.set_header("Content-Type", "application/json");
        CorsMiddleware::add_headers(res);
        return res;
    });
    
    // ============================================================
    // AUTHENTICATION ENDPOINTS
    // ============================================================
    CROW_ROUTE(app, "/api/auth/register").methods("POST"_method)
    ([&auth_controller](const crow::request& req){
        LoggerMiddleware::log_request(req);
        auto res = auth_controller.register_user(req);
        LoggerMiddleware::log_response(res, res.code);
        CorsMiddleware::add_headers(res);
        return res;
    });
    
    CROW_ROUTE(app, "/api/auth/login").methods("POST"_method)
    ([&auth_controller](const crow::request& req){
        LoggerMiddleware::log_request(req);
        auto res = auth_controller.login(req);
        LoggerMiddleware::log_response(res, res.code);
        CorsMiddleware::add_headers(res);
        return res;
    });
    
    CROW_ROUTE(app, "/api/auth/me").methods("GET"_method)
    ([&auth_controller](const crow::request& req){
        LoggerMiddleware::log_request(req);
        auto res = auth_controller.get_profile(req);
        LoggerMiddleware::log_response(res, res.code);
        CorsMiddleware::add_headers(res);
        return res;
    });
    
    CROW_ROUTE(app, "/api/auth/profile").methods("PUT"_method)
    ([&auth_controller](const crow::request& req){
        LoggerMiddleware::log_request(req);
        auto res = auth_controller.update_profile(req);
        LoggerMiddleware::log_response(res, res.code);
        CorsMiddleware::add_headers(res);
        return res;
    });
    
    CROW_ROUTE(app, "/api/auth/change-password").methods("POST"_method)
    ([&auth_controller](const crow::request& req){
        LoggerMiddleware::log_request(req);
        auto res = auth_controller.change_password(req);
        LoggerMiddleware::log_response(res, res.code);
        CorsMiddleware::add_headers(res);
        return res;
    });
    
    // ============================================================
    // SHIPMENT ENDPOINTS
    // ============================================================
    CROW_ROUTE(app, "/api/shipments").methods("POST"_method)
    ([&shipment_controller](const crow::request& req){
        LoggerMiddleware::log_request(req);
        auto res = shipment_controller.create_shipment(req);
        LoggerMiddleware::log_response(res, res.code);
        CorsMiddleware::add_headers(res);
        return res;
    });
    
    CROW_ROUTE(app, "/api/shipments").methods("GET"_method)
    ([&shipment_controller](const crow::request& req){
        LoggerMiddleware::log_request(req);
        auto res = shipment_controller.get_shipments(req);
        LoggerMiddleware::log_response(res, res.code);
        CorsMiddleware::add_headers(res);
        return res;
    });
    
    CROW_ROUTE(app, "/api/shipments/<int>").methods("GET"_method)
    ([&shipment_controller](const crow::request& req, int shipment_id){
        LoggerMiddleware::log_request(req);
        auto res = shipment_controller.get_shipment(req, shipment_id);
        LoggerMiddleware::log_response(res, res.code);
        CorsMiddleware::add_headers(res);
        return res;
    });
    
    CROW_ROUTE(app, "/api/shipments/<int>/tracking").methods("GET"_method)
    ([&shipment_controller](const crow::request& req, int shipment_id){
        LoggerMiddleware::log_request(req);
        auto res = shipment_controller.get_tracking(req, shipment_id);
        LoggerMiddleware::log_response(res, res.code);
        CorsMiddleware::add_headers(res);
        return res;
    });
    
    CROW_ROUTE(app, "/api/shipments/price-estimate").methods("POST"_method)
    ([&shipment_controller](const crow::request& req){
        LoggerMiddleware::log_request(req);
        auto res = shipment_controller.price_estimate(req);
        LoggerMiddleware::log_response(res, res.code);
        CorsMiddleware::add_headers(res);
        return res;
    });
    
    CROW_ROUTE(app, "/api/shipments/<int>").methods("DELETE"_method)
    ([&shipment_controller](const crow::request& req, int shipment_id){
        LoggerMiddleware::log_request(req);
        auto res = shipment_controller.cancel_shipment(req, shipment_id);
        LoggerMiddleware::log_response(res, res.code);
        CorsMiddleware::add_headers(res);
        return res;
    });
    
    // ============================================================
    // AGENT ENDPOINTS
    // ============================================================
    CROW_ROUTE(app, "/api/agent/deliveries").methods("GET"_method)
    ([&agent_controller](const crow::request& req){
        LoggerMiddleware::log_request(req);
        auto res = agent_controller.get_deliveries(req);
        LoggerMiddleware::log_response(res, res.code);
        CorsMiddleware::add_headers(res);
        return res;
    });
    
    CROW_ROUTE(app, "/api/agent/deliveries/<int>").methods("GET"_method)
    ([&agent_controller](const crow::request& req, int shipment_id){
        LoggerMiddleware::log_request(req);
        auto res = agent_controller.get_delivery(req, shipment_id);
        LoggerMiddleware::log_response(res, res.code);
        CorsMiddleware::add_headers(res);
        return res;
    });
    
    CROW_ROUTE(app, "/api/agent/deliveries/<int>/status").methods("PATCH"_method)
    ([&agent_controller](const crow::request& req, int shipment_id){
        LoggerMiddleware::log_request(req);
        auto res = agent_controller.update_status(req, shipment_id);
        LoggerMiddleware::log_response(res, res.code);
        CorsMiddleware::add_headers(res);
        return res;
    });
    
    CROW_ROUTE(app, "/api/agent/deliveries/<int>/delivered").methods("POST"_method)
    ([&agent_controller](const crow::request& req, int shipment_id){
        LoggerMiddleware::log_request(req);
        auto res = agent_controller.mark_delivered(req, shipment_id);
        LoggerMiddleware::log_response(res, res.code);
        CorsMiddleware::add_headers(res);
        return res;
    });
    
    CROW_ROUTE(app, "/api/agent/deliveries/<int>/failed").methods("POST"_method)
    ([&agent_controller](const crow::request& req, int shipment_id){
        LoggerMiddleware::log_request(req);
        auto res = agent_controller.mark_failed(req, shipment_id);
        LoggerMiddleware::log_response(res, res.code);
        CorsMiddleware::add_headers(res);
        return res;
    });
    
    CROW_ROUTE(app, "/api/agent/stats").methods("GET"_method)
    ([&agent_controller](const crow::request& req){
        LoggerMiddleware::log_request(req);
        auto res = agent_controller.get_stats(req);
        LoggerMiddleware::log_response(res, res.code);
        CorsMiddleware::add_headers(res);
        return res;
    });
    
    // ============================================================
    // ADMIN ENDPOINTS
    // ============================================================
    CROW_ROUTE(app, "/api/admin/users").methods("GET"_method)
    ([&admin_controller](const crow::request& req){
        LoggerMiddleware::log_request(req);
        auto res = admin_controller.get_users(req);
        LoggerMiddleware::log_response(res, res.code);
        CorsMiddleware::add_headers(res);
        return res;
    });
    
    CROW_ROUTE(app, "/api/admin/users").methods("POST"_method)
    ([&admin_controller](const crow::request& req){
        LoggerMiddleware::log_request(req);
        auto res = admin_controller.create_user(req);
        LoggerMiddleware::log_response(res, res.code);
        CorsMiddleware::add_headers(res);
        return res;
    });
    
    CROW_ROUTE(app, "/api/admin/users/<int>").methods("PUT"_method)
    ([&admin_controller](const crow::request& req, int user_id){
        LoggerMiddleware::log_request(req);
        auto res = admin_controller.update_user(req, user_id);
        LoggerMiddleware::log_response(res, res.code);
        CorsMiddleware::add_headers(res);
        return res;
    });
    
    CROW_ROUTE(app, "/api/admin/users/<int>/toggle").methods("PATCH"_method)
    ([&admin_controller](const crow::request& req, int user_id){
        LoggerMiddleware::log_request(req);
        auto res = admin_controller.toggle_user(req, user_id);
        LoggerMiddleware::log_response(res, res.code);
        CorsMiddleware::add_headers(res);
        return res;
    });
    
    CROW_ROUTE(app, "/api/admin/shipments").methods("GET"_method)
    ([&admin_controller](const crow::request& req){
        LoggerMiddleware::log_request(req);
        auto res = admin_controller.get_all_shipments(req);
        LoggerMiddleware::log_response(res, res.code);
        CorsMiddleware::add_headers(res);
        return res;
    });
    
    CROW_ROUTE(app, "/api/admin/shipments/<int>/assign").methods("PATCH"_method)
    ([&admin_controller](const crow::request& req, int shipment_id){
        LoggerMiddleware::log_request(req);
        auto res = admin_controller.assign_agent(req, shipment_id);
        LoggerMiddleware::log_response(res, res.code);
        CorsMiddleware::add_headers(res);
        return res;
    });
    
    CROW_ROUTE(app, "/api/admin/shipments/<int>/status").methods("PATCH"_method)
    ([&admin_controller](const crow::request& req, int shipment_id){
        LoggerMiddleware::log_request(req);
        auto res = admin_controller.update_shipment_status(req, shipment_id);
        LoggerMiddleware::log_response(res, res.code);
        CorsMiddleware::add_headers(res);
        return res;
    });
    
    CROW_ROUTE(app, "/api/admin/hubs").methods("GET"_method)
    ([&admin_controller](const crow::request& req){
        LoggerMiddleware::log_request(req);
        auto res = admin_controller.get_hubs(req);
        LoggerMiddleware::log_response(res, res.code);
        CorsMiddleware::add_headers(res);
        return res;
    });
    
    CROW_ROUTE(app, "/api/admin/hubs").methods("POST"_method)
    ([&admin_controller](const crow::request& req){
        LoggerMiddleware::log_request(req);
        auto res = admin_controller.create_hub(req);
        LoggerMiddleware::log_response(res, res.code);
        CorsMiddleware::add_headers(res);
        return res;
    });
    
    CROW_ROUTE(app, "/api/admin/routes").methods("GET"_method)
    ([&admin_controller](const crow::request& req){
        LoggerMiddleware::log_request(req);
        auto res = admin_controller.get_routes(req);
        LoggerMiddleware::log_response(res, res.code);
        CorsMiddleware::add_headers(res);
        return res;
    });
    
    // ============================================================
    // ANALYTICS ENDPOINTS
    // ============================================================
    CROW_ROUTE(app, "/api/analytics/dashboard").methods("GET"_method)
    ([&analytics_controller](const crow::request& req){
        LoggerMiddleware::log_request(req);
        auto res = analytics_controller.get_dashboard(req);
        LoggerMiddleware::log_response(res, res.code);
        CorsMiddleware::add_headers(res);
        return res;
    });
    
    CROW_ROUTE(app, "/api/analytics/shipments").methods("GET"_method)
    ([&analytics_controller](const crow::request& req){
        LoggerMiddleware::log_request(req);
        auto res = analytics_controller.get_shipment_trends(req);
        LoggerMiddleware::log_response(res, res.code);
        CorsMiddleware::add_headers(res);
        return res;
    });
    
    CROW_ROUTE(app, "/api/analytics/revenue").methods("GET"_method)
    ([&analytics_controller](const crow::request& req){
        LoggerMiddleware::log_request(req);
        auto res = analytics_controller.get_revenue(req);
        LoggerMiddleware::log_response(res, res.code);
        CorsMiddleware::add_headers(res);
        return res;
    });
    
    CROW_ROUTE(app, "/api/analytics/agents").methods("GET"_method)
    ([&analytics_controller](const crow::request& req){
        LoggerMiddleware::log_request(req);
        auto res = analytics_controller.get_agent_performance(req);
        LoggerMiddleware::log_response(res, res.code);
        CorsMiddleware::add_headers(res);
        return res;
    });
    
    CROW_ROUTE(app, "/api/analytics/routes").methods("GET"_method)
    ([&analytics_controller](const crow::request& req){
        LoggerMiddleware::log_request(req);
        auto res = analytics_controller.get_popular_routes(req);
        LoggerMiddleware::log_response(res, res.code);
        CorsMiddleware::add_headers(res);
        return res;
    });
    
    // ============================================================
    // OPTIONS handler for CORS preflight
    // ============================================================
    CROW_ROUTE(app, "/<path>")
    ([](const crow::request& req, std::string path){
        if (req.method == "OPTIONS"_method) {
            return CorsMiddleware::handle_options();
        }
        crow::response res(404, "Not found");
        CorsMiddleware::add_headers(res);
        return res;
    });
    
    // ============================================================
    // START SERVER
    // ============================================================
    std::cout << "\nStarting server on port " << port << "..." << std::endl;
    std::cout << "Health check: http://localhost:" << port << "/api/health" << std::endl;
    std::cout << "API Base: http://localhost:" << port << "/api" << std::endl;
    std::cout << "\nPress Ctrl+C to stop the server\n" << std::endl;
    
    app.bindaddr("0.0.0.0").port(Config::get_port()).multithreaded().run();
    
    return 0;
}
