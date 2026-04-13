// include/controllers/AdminController.h
#pragma once
#include <crow.h>
#include "../database/UserRepository.h"
#include "../database/ShipmentRepository.h"

class AdminController {
public:
    // Get all users (requires auth: ADMIN)
    crow::response get_users(const crow::request& req);
    
    // Create user (requires auth: ADMIN)
    crow::response create_user(const crow::request& req);
    
    // Update user (requires auth: ADMIN)
    crow::response update_user(const crow::request& req, int user_id);
    
    // Toggle user active status (requires auth: ADMIN)
    crow::response toggle_user(const crow::request& req, int user_id);
    
    // Get all shipments (requires auth: ADMIN)
    crow::response get_all_shipments(const crow::request& req);
    
    // Assign agent to shipment (requires auth: ADMIN)
    crow::response assign_agent(const crow::request& req, int shipment_id);
    
    // Manually update shipment status (requires auth: ADMIN)
    crow::response update_shipment_status(const crow::request& req, int shipment_id);
    
    // Get all hubs (requires auth: ADMIN)
    crow::response get_hubs(const crow::request& req);
    
    // Create hub (requires auth: ADMIN)
    crow::response create_hub(const crow::request& req);
    
    // Get all routes (requires auth: ADMIN)
    crow::response get_routes(const crow::request& req);
};
