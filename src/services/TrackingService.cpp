// src/services/TrackingService.cpp
#include "../../include/services/TrackingService.h"
#include "../../include/database/TrackingRepository.h"
#include "../../include/database/ShipmentRepository.h"
#include "../../include/utils/DateTimeUtils.h"

bool TrackingService::add_event(int shipment_id, const std::string& status, const std::string& location, 
                                 const std::string& description, int updated_by, bool is_automated) {
    TrackingEvent event;
    event.shipment_id = shipment_id;
    event.status = status;
    event.location = location;
    event.description = description;
    event.updated_by = updated_by;
    event.is_automated = is_automated;
    event.created_at = DateTimeUtils::now();
    
    return TrackingRepository::create(event);
}

std::vector<TrackingEvent> TrackingService::get_history(int shipment_id) {
    return TrackingRepository::get_by_shipment_id(shipment_id);
}

std::optional<TrackingEvent> TrackingService::get_latest_status(int shipment_id) {
    return TrackingRepository::get_latest(shipment_id);
}

nlohmann::json TrackingService::get_public_tracking(const std::string& tracking_number) {
    auto shipment = ShipmentRepository::find_by_tracking_number(tracking_number);
    if (!shipment) {
        nlohmann::json error = {{"error", "Tracking number not found"}};
        return error;
    }
    
    auto history = TrackingRepository::get_by_shipment_id(shipment->id);
    
    nlohmann::json result;
    result["tracking_number"] = shipment->tracking_number;
    result["current_status"] = shipment->current_status;
    result["current_location"] = shipment->current_location;
    result["estimated_delivery_at"] = shipment->estimated_delivery_at;
    result["sender_city"] = shipment->sender_city;
    result["recipient_city"] = shipment->recipient_city;
    result["service_type"] = shipment->service_type;
    result["package_type"] = shipment->package_type;
    
    nlohmann::json timeline = nlohmann::json::array();
    for (const auto& event : history) {
        timeline.push_back(event.to_json());
    }
    result["timeline"] = timeline;
    
    return result;
}
