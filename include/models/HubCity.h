// include/models/HubCity.h
#pragma once
#include <string>
#include <nlohmann/json.hpp>

struct HubCity {
    int id = 0;
    std::string city_name;
    std::string state;
    std::string hub_code;
    double latitude = 0;
    double longitude = 0;
    bool is_active = true;
    std::string created_at;
    
    nlohmann::json to_json() const {
        return {
            {"id", id},
            {"city_name", city_name},
            {"state", state},
            {"hub_code", hub_code},
            {"latitude", latitude},
            {"longitude", longitude},
            {"is_active", is_active},
            {"created_at", created_at}
        };
    }
};
