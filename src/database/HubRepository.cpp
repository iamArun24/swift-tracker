// src/database/HubRepository.cpp
#include "../../include/database/HubRepository.h"
#include "../../include/database/DatabaseManager.h"
#include <sstream>

bool HubRepository::create(const HubCity& hub) {
    auto& db = DatabaseManager::get_instance();
    
    std::ostringstream sql;
    sql << "INSERT INTO hub_cities (city_name, state, hub_code, latitude, longitude, is_active, created_at) VALUES ('"
        << hub.city_name << "', '"
        << hub.state << "', '"
        << hub.hub_code << "', "
        << hub.latitude << ", "
        << hub.longitude << ", "
        << (hub.is_active ? 1 : 0) << ", '"
        << hub.created_at << "')";
    
    return db.execute(sql.str());
}

std::optional<HubCity> HubRepository::find_by_id(int id) {
    auto& db = DatabaseManager::get_instance();
    
    std::ostringstream sql;
    sql << "SELECT * FROM hub_cities WHERE id = " << id;
    
    auto result = db.query_one(sql.str());
    if (result.empty()) {
        return std::nullopt;
    }
    
    HubCity hub;
    hub.id = std::stoi(result["id"]);
    hub.city_name = result["city_name"];
    hub.state = result["state"];
    hub.hub_code = result["hub_code"];
    hub.latitude = std::stod(result["latitude"]);
    hub.longitude = std::stod(result["longitude"]);
    hub.is_active = result["is_active"] == "1";
    hub.created_at = result["created_at"];
    
    return hub;
}

std::optional<HubCity> HubRepository::find_by_city_name(const std::string& city_name) {
    auto& db = DatabaseManager::get_instance();
    
    std::ostringstream sql;
    sql << "SELECT * FROM hub_cities WHERE LOWER(city_name) = LOWER('" << city_name << "')";
    
    auto result = db.query_one(sql.str());
    if (result.empty()) {
        return std::nullopt;
    }
    
    HubCity hub;
    hub.id = std::stoi(result["id"]);
    hub.city_name = result["city_name"];
    hub.state = result["state"];
    hub.hub_code = result["hub_code"];
    hub.latitude = std::stod(result["latitude"]);
    hub.longitude = std::stod(result["longitude"]);
    hub.is_active = result["is_active"] == "1";
    hub.created_at = result["created_at"];
    
    return hub;
}

std::optional<HubCity> HubRepository::find_by_hub_code(const std::string& hub_code) {
    auto& db = DatabaseManager::get_instance();
    
    std::ostringstream sql;
    sql << "SELECT * FROM hub_cities WHERE hub_code = '" << hub_code << "'";
    
    auto result = db.query_one(sql.str());
    if (result.empty()) {
        return std::nullopt;
    }
    
    HubCity hub;
    hub.id = std::stoi(result["id"]);
    hub.city_name = result["city_name"];
    hub.state = result["state"];
    hub.hub_code = result["hub_code"];
    hub.latitude = std::stod(result["latitude"]);
    hub.longitude = std::stod(result["longitude"]);
    hub.is_active = result["is_active"] == "1";
    hub.created_at = result["created_at"];
    
    return hub;
}

bool HubRepository::update(const HubCity& hub) {
    auto& db = DatabaseManager::get_instance();
    
    std::ostringstream sql;
    sql << "UPDATE hub_cities SET city_name = '" << hub.city_name
        << "', state = '" << hub.state
        << "', hub_code = '" << hub.hub_code
        << "', latitude = " << hub.latitude
        << ", longitude = " << hub.longitude
        << ", is_active = " << (hub.is_active ? 1 : 0)
        << " WHERE id = " << hub.id;
    
    return db.execute(sql.str());
}

std::vector<HubCity> HubRepository::get_all(bool active_only) {
    auto& db = DatabaseManager::get_instance();
    
    std::ostringstream sql;
    sql << "SELECT * FROM hub_cities";
    
    if (active_only) {
        sql << " WHERE is_active = 1";
    }
    
    sql << " ORDER BY city_name";
    
    auto results = db.query(sql.str());
    std::vector<HubCity> hubs;
    
    for (const auto& row : results) {
        HubCity hub;
        hub.id = std::stoi(row.at("id"));
        hub.city_name = row.at("city_name");
        hub.state = row.at("state");
        hub.hub_code = row.at("hub_code");
        hub.latitude = std::stod(row.at("latitude"));
        hub.longitude = std::stod(row.at("longitude"));
        hub.is_active = row.at("is_active") == "1";
        hub.created_at = row.at("created_at");
        hubs.push_back(hub);
    }
    
    return hubs;
}

bool HubRepository::remove(int id) {
    auto& db = DatabaseManager::get_instance();
    
    std::ostringstream sql;
    sql << "DELETE FROM hub_cities WHERE id = " << id;
    
    return db.execute(sql.str());
}
