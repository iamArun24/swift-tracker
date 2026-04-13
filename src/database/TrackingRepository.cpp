// src/database/TrackingRepository.cpp
#include "../../include/database/TrackingRepository.h"
#include "../../include/database/DatabaseManager.h"
#include <sstream>

bool TrackingRepository::create(const TrackingEvent& event) {
    auto& db = DatabaseManager::get_instance();
    
    std::ostringstream sql;
    sql << "INSERT INTO tracking_events (shipment_id, status, location, latitude, longitude, "
        << "description, updated_by, is_automated, created_at) VALUES ("
        << event.shipment_id << ", '"
        << event.status << "', '"
        << event.location << "', "
        << event.latitude << ", "
        << event.longitude << ", '"
        << event.description << "', "
        << event.updated_by << ", "
        << (event.is_automated ? 1 : 0) << ", '"
        << event.created_at << "')";
    
    return db.execute(sql.str());
}

std::vector<TrackingEvent> TrackingRepository::get_by_shipment_id(int shipment_id) {
    auto& db = DatabaseManager::get_instance();
    
    std::ostringstream sql;
    sql << "SELECT * FROM tracking_events WHERE shipment_id = " << shipment_id
        << " ORDER BY created_at ASC";
    
    auto results = db.query(sql.str());
    std::vector<TrackingEvent> events;
    
    for (const auto& row : results) {
        TrackingEvent event;
        event.id = std::stoi(row.at("id"));
        event.shipment_id = std::stoi(row.at("shipment_id"));
        event.status = row.at("status");
        event.location = row.at("location");
        event.latitude = row.at("latitude").empty() ? 0 : std::stod(row.at("latitude"));
        event.longitude = row.at("longitude").empty() ? 0 : std::stod(row.at("longitude"));
        event.description = row.at("description");
        event.updated_by = row.at("updated_by").empty() ? 0 : std::stoi(row.at("updated_by"));
        event.is_automated = row.at("is_automated") == "1";
        event.created_at = row.at("created_at");
        events.push_back(event);
    }
    
    return events;
}

std::optional<TrackingEvent> TrackingRepository::get_latest(int shipment_id) {
    auto& db = DatabaseManager::get_instance();
    
    std::ostringstream sql;
    sql << "SELECT * FROM tracking_events WHERE shipment_id = " << shipment_id
        << " ORDER BY created_at DESC LIMIT 1";
    
    auto result = db.query_one(sql.str());
    if (result.empty()) {
        return std::nullopt;
    }
    
    TrackingEvent event;
    event.id = std::stoi(result["id"]);
    event.shipment_id = std::stoi(result["shipment_id"]);
    event.status = result["status"];
    event.location = result["location"];
    event.latitude = result["latitude"].empty() ? 0 : std::stod(result["latitude"]);
    event.longitude = result["longitude"].empty() ? 0 : std::stod(result["longitude"]);
    event.description = result["description"];
    event.updated_by = result["updated_by"].empty() ? 0 : std::stoi(result["updated_by"]);
    event.is_automated = result["is_automated"] == "1";
    event.created_at = result["created_at"];
    
    return event;
}

std::vector<TrackingEvent> TrackingRepository::get_by_status(int shipment_id, const std::string& status) {
    auto& db = DatabaseManager::get_instance();
    
    std::ostringstream sql;
    sql << "SELECT * FROM tracking_events WHERE shipment_id = " << shipment_id
        << " AND status = '" << status << "' ORDER BY created_at ASC";
    
    auto results = db.query(sql.str());
    std::vector<TrackingEvent> events;
    
    for (const auto& row : results) {
        TrackingEvent event;
        event.id = std::stoi(row.at("id"));
        event.shipment_id = std::stoi(row.at("shipment_id"));
        event.status = row.at("status");
        event.location = row.at("location");
        event.latitude = row.at("latitude").empty() ? 0 : std::stod(row.at("latitude"));
        event.longitude = row.at("longitude").empty() ? 0 : std::stod(row.at("longitude"));
        event.description = row.at("description");
        event.updated_by = row.at("updated_by").empty() ? 0 : std::stoi(row.at("updated_by"));
        event.is_automated = row.at("is_automated") == "1";
        event.created_at = row.at("created_at");
        events.push_back(event);
    }
    
    return events;
}
