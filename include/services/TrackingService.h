// include/services/TrackingService.h
#pragma once
#include <optional>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include "../models/TrackingEvent.h"

class TrackingService {
public:
    // Add tracking event
    static bool add_event(int shipment_id, const std::string& status, const std::string& location, 
                         const std::string& description, int updated_by = 0, bool is_automated = false);
    
    // Get tracking history
    static std::vector<TrackingEvent> get_history(int shipment_id);
    
    // Get latest status
    static std::optional<TrackingEvent> get_latest_status(int shipment_id);
    
    // Get public tracking data (no auth required)
    static nlohmann::json get_public_tracking(const std::string& tracking_number);
};
