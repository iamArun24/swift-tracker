// include/models/Route.h
#pragma once
#include <string>
#include <nlohmann/json.hpp>

struct Route {
    int id = 0;
    int from_hub_id = 0;
    int to_hub_id = 0;
    double distance_km = 0;
    int transit_hours = 0;
    bool is_active = true;
    std::string created_at;
    
    nlohmann::json to_json() const {
        return {
            {"id", id},
            {"from_hub_id", from_hub_id},
            {"to_hub_id", to_hub_id},
            {"distance_km", distance_km},
            {"transit_hours", transit_hours},
            {"is_active", is_active},
            {"created_at", created_at}
        };
    }
};
