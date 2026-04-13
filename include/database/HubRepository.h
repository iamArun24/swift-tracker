// include/database/HubRepository.h
#pragma once
#include <string>
#include <vector>
#include <optional>
#include <nlohmann/json.hpp>
#include "../models/HubCity.h"

class HubRepository {
public:
    // Create hub
    static bool create(const HubCity& hub);
    
    // Find hub by ID
    static std::optional<HubCity> find_by_id(int id);
    
    // Find hub by city name
    static std::optional<HubCity> find_by_city_name(const std::string& city_name);
    
    // Find hub by hub code
    static std::optional<HubCity> find_by_hub_code(const std::string& hub_code);
    
    // Update hub
    static bool update(const HubCity& hub);
    
    // Get all hubs
    static std::vector<HubCity> get_all(bool active_only = true);
    
    // Delete hub
    static bool remove(int id);
};
