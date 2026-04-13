// include/services/ShipmentService.h
#pragma once
#include <string>
#include <vector>
#include <optional>
#include <nlohmann/json.hpp>
#include "../models/Shipment.h"
#include "../utils/PriceCalculator.h"

class ShipmentService {
public:
    // Create new shipment
    static std::optional<Shipment> create_shipment(int sender_id, const CreateShipmentRequest& request);
    
    // Get shipment by ID
    static std::optional<Shipment> get_shipment(int shipment_id);
    
    // Get shipment by tracking number
    static std::optional<Shipment> get_by_tracking_number(const std::string& tracking_number);
    
    // Get shipments for sender
    static std::vector<Shipment> get_sender_shipments(int sender_id, int limit = 50, int offset = 0);
    
    // Calculate price estimate
    static PriceBreakdown calculate_price_estimate(const CreateShipmentRequest& request, double distance_km);
    
    // Cancel shipment
    static bool cancel_shipment(int shipment_id);
    
    // Update shipment status
    static bool update_status(int shipment_id, const std::string& status, const std::string& location, int updated_by);
};
