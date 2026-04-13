// include/database/ShipmentRepository.h
#pragma once
#include <string>
#include <vector>
#include <optional>
#include <nlohmann/json.hpp>
#include "../models/Shipment.h"

class ShipmentRepository {
public:
    // Create new shipment
    static bool create(const Shipment& shipment);
    
    // Find shipment by ID
    static std::optional<Shipment> find_by_id(int id);
    
    // Find shipment by tracking number
    static std::optional<Shipment> find_by_tracking_number(const std::string& tracking_number);
    
    // Update shipment
    static bool update(const Shipment& shipment);
    
    // Update status
    static bool update_status(int shipment_id, const std::string& status, const std::string& location = "");
    
    // Assign agent
    static bool assign_agent(int shipment_id, int agent_id);
    
    // Get shipments by sender ID
    static std::vector<Shipment> get_by_sender(int sender_id, int limit = 50, int offset = 0);
    
    // Get shipments by agent ID
    static std::vector<Shipment> get_by_agent(int agent_id, int limit = 50, int offset = 0);
    
    // Get all shipments (for admin)
    static std::vector<Shipment> get_all(int limit = 100, int offset = 0, const std::string& status_filter = "");
    
    // Get count
    static int get_count(const std::string& status_filter = "");
    
    // Get count by sender
    static int get_count_by_sender(int sender_id);
    
    // Get count by agent
    static int get_count_by_agent(int agent_id);
};
