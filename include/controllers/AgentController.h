// include/controllers/AgentController.h
#pragma once
#include <crow.h>
#include "../services/AgentService.h"

class AgentController {
private:
    AgentService agent_service_;
    
public:
    // Get my deliveries (requires auth: AGENT)
    crow::response get_deliveries(const crow::request& req);
    
    // Get single delivery details (requires auth: AGENT)
    crow::response get_delivery(const crow::request& req, int shipment_id);
    
    // Update delivery status (requires auth: AGENT)
    crow::response update_status(const crow::request& req, int shipment_id);
    
    // Mark as delivered (requires auth: AGENT)
    crow::response mark_delivered(const crow::request& req, int shipment_id);
    
    // Mark as failed (requires auth: AGENT)
    crow::response mark_failed(const crow::request& req, int shipment_id);
    
    // Get my stats (requires auth: AGENT)
    crow::response get_stats(const crow::request& req);
};
