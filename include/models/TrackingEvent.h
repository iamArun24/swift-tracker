// include/models/TrackingEvent.h
#pragma once
#include <string>
#include <nlohmann/json.hpp>

struct TrackingEvent {
    int id = 0;
    int shipment_id = 0;
    std::string status;
    std::string location;
    double latitude = 0;
    double longitude = 0;
    std::string description;
    int updated_by = 0;
    bool is_automated = false;
    std::string created_at;
    
    nlohmann::json to_json() const {
        return {
            {"id", id},
            {"shipment_id", shipment_id},
            {"status", status},
            {"location", location},
            {"latitude", latitude},
            {"longitude", longitude},
            {"description", description},
            {"updated_by", updated_by},
            {"is_automated", is_automated},
            {"created_at", created_at}
        };
    }
};
