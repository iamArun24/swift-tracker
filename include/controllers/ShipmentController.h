// include/controllers/ShipmentController.h
#pragma once
#include <crow.h>
#include "../services/ShipmentService.h"

class ShipmentController {
private:
    ShipmentService shipment_service_;
    
public:
    // Create new shipment (requires auth: SENDER or ADMIN)
    crow::response create_shipment(const crow::request& req);
    
    // Get my shipments (requires auth)
    crow::response get_shipments(const crow::request& req);
    
    // Get shipment by ID (requires auth)
    crow::response get_shipment(const crow::request& req, int shipment_id);
    
    // Get shipment tracking history (requires auth)
    crow::response get_tracking(const crow::request& req, int shipment_id);
    
    // Calculate price estimate (requires auth)
    crow::response price_estimate(const crow::request& req);
    
    // Cancel shipment (requires auth)
    crow::response cancel_shipment(const crow::request& req, int shipment_id);
};
