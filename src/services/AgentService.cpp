// src/services/AgentService.cpp
#include "../../include/services/AgentService.h"
#include "../../include/database/ShipmentRepository.h"
#include "../../include/database/TrackingRepository.h"
#include "../../include/utils/DateTimeUtils.h"

std::vector<Shipment> AgentService::get_agent_deliveries(int agent_id, int limit, int offset) {
    return ShipmentRepository::get_by_agent(agent_id, limit, offset);
}

std::optional<Shipment> AgentService::get_delivery_details(int agent_id, int shipment_id) {
    auto shipment = ShipmentRepository::find_by_id(shipment_id);
    
    if (!shipment || shipment->assigned_agent_id != agent_id) {
        return std::nullopt;
    }
    
    return shipment;
}

bool AgentService::update_delivery_status(int agent_id, int shipment_id, const std::string& status, 
                                          const std::string& location, const std::string& notes) {
    auto shipment = ShipmentRepository::find_by_id(shipment_id);
    
    if (!shipment || shipment->assigned_agent_id != agent_id) {
        return false;
    }
    
    // Update shipment status
    if (!ShipmentRepository::update_status(shipment_id, status, location)) {
        return false;
    }
    
    // Add tracking event
    std::string description = status;
    if (!notes.empty()) {
        description += ". " + notes;
    }
    
    TrackingRepository::create({
        0,
        shipment_id,
        status,
        location,
        0,
        0,
        description,
        agent_id,
        false,
        DateTimeUtils::now()
    });
    
    return true;
}

bool AgentService::mark_delivered(int agent_id, int shipment_id, const std::string& location, 
                                  const std::string& notes, const std::string& recipient_name) {
    auto shipment = ShipmentRepository::find_by_id(shipment_id);
    
    if (!shipment || shipment->assigned_agent_id != agent_id) {
        return false;
    }
    
    // Update status to DELIVERED
    if (!ShipmentRepository::update_status(shipment_id, "DELIVERED", location)) {
        return false;
    }
    
    // Add tracking event
    std::string description = "Package delivered successfully";
    if (!recipient_name.empty()) {
        description += " to " + recipient_name;
    }
    if (!notes.empty()) {
        description += ". " + notes;
    }
    
    TrackingRepository::create({
        0,
        shipment_id,
        "DELIVERED",
        location,
        0,
        0,
        description,
        agent_id,
        false,
        DateTimeUtils::now()
    });
    
    return true;
}

bool AgentService::mark_failed(int agent_id, int shipment_id, const std::string& location, 
                               const std::string& reason) {
    auto shipment = ShipmentRepository::find_by_id(shipment_id);
    
    if (!shipment || shipment->assigned_agent_id != agent_id) {
        return false;
    }
    
    // Update status to FAILED_DELIVERY
    if (!ShipmentRepository::update_status(shipment_id, "FAILED_DELIVERY", location)) {
        return false;
    }
    
    // Add tracking event
    std::string description = "Delivery failed: " + reason;
    
    TrackingRepository::create({
        0,
        shipment_id,
        "FAILED_DELIVERY",
        location,
        0,
        0,
        description,
        agent_id,
        false,
        DateTimeUtils::now()
    });
    
    return true;
}

nlohmann::json AgentService::get_agent_stats(int agent_id) {
    auto shipments = ShipmentRepository::get_by_agent(agent_id, 1000, 0);
    
    int total = shipments.size();
    int delivered = 0;
    int failed = 0;
    
    for (const auto& s : shipments) {
        if (s.current_status == "DELIVERED") {
            delivered++;
        } else if (s.current_status == "FAILED_DELIVERY") {
            failed++;
        }
    }
    
    double success_rate = total > 0 ? (double)delivered / total * 100.0 : 0.0;
    
    nlohmann::json stats = {
        {"total_deliveries", total},
        {"successful_deliveries", delivered},
        {"failed_deliveries", failed},
        {"success_rate", success_rate},
        {"pending_deliveries", total - delivered - failed}
    };
    
    return stats;
}
