// include/services/RouteEngine.h
#pragma once
#include <string>
#include <vector>
#include <optional>
#include <nlohmann/json.hpp>

struct HubNode {
    int id;
    std::string city_name;
    std::string hub_code;
    double latitude;
    double longitude;
};

struct RouteResult {
    bool found = false;
    std::vector<int> hub_ids;
    std::vector<std::string> hub_names;
    double total_distance_km = 0;
    int total_transit_hours = 0;
    std::string route_path_json;
    
    nlohmann::json to_json() const {
        return {
            {"found", found},
            {"hub_ids", hub_ids},
            {"hub_names", hub_names},
            {"total_distance_km", total_distance_km},
            {"total_transit_hours", total_transit_hours},
            {"route_path_json", route_path_json}
        };
    }
};

class RouteEngine {
public:
    static void initialize();
    
    // Find optimal route between two cities
    static RouteResult find_optimal_route(const std::string& from_city, const std::string& to_city);
    
    // Find hub by city name
    static std::optional<HubNode> find_hub_by_city(const std::string& city_name);
    
    // Get all hubs (for frontend map)
    static nlohmann::json get_all_hubs_json();
    
    // Reload graph from database
    static void reload();
};
