// include/services/AgentService.h
#pragma once
#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include "../models/Shipment.h"

class AgentService {
public:
    // Get deliveries assigned to agent
    static std::vector<Shipment> get_agent_deliveries(int agent_id, int limit = 50, int offset = 0);
    
    // Get single delivery details
    static std::optional<Shipment> get_delivery_details(int agent_id, int shipment_id);
    
    // Update delivery status
    static bool update_delivery_status(int agent_id, int shipment_id, const std::string& status, 
                                      const std::string& location, const std::string& notes = "");
    
    // Mark as delivered
    static bool mark_delivered(int agent_id, int shipment_id, const std::string& location, 
                              const std::string& notes = "", const std::string& recipient_name = "");
    
    // Mark as failed delivery
    static bool mark_failed(int agent_id, int shipment_id, const std::string& location, 
                           const std::string& reason);
    
    // Get agent stats
    static nlohmann::json get_agent_stats(int agent_id);
};
