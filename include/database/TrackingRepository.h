// include/database/TrackingRepository.h
#pragma once
#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include "../models/TrackingEvent.h"

class TrackingRepository {
public:
    // Create tracking event
    static bool create(const TrackingEvent& event);
    
    // Get tracking history for shipment
    static std::vector<TrackingEvent> get_by_shipment_id(int shipment_id);
    
    // Get latest tracking event
    static std::optional<TrackingEvent> get_latest(int shipment_id);
    
    // Get tracking events by status
    static std::vector<TrackingEvent> get_by_status(int shipment_id, const std::string& status);
};
