// src/database/ShipmentRepository.cpp
#include "../../include/database/ShipmentRepository.h"
#include "../../include/database/DatabaseManager.h"
#include "../../include/utils/DateTimeUtils.h"
#include <sstream>

bool ShipmentRepository::create(const Shipment& shipment) {
    auto& db = DatabaseManager::get_instance();
    
    std::ostringstream sql;
    sql << "INSERT INTO shipments (tracking_number, sender_id, "
        << "sender_name, sender_phone, sender_email, sender_address, sender_city, sender_state, sender_pincode, "
        << "recipient_name, recipient_phone, recipient_email, recipient_address, recipient_city, recipient_state, recipient_pincode, "
        << "package_type, weight_grams, length_cm, width_cm, height_cm, package_description, declared_value, "
        << "service_type, origin_hub_id, destination_hub_id, route_path, "
        << "current_status, current_location, "
        << "base_cost, tax_amount, total_cost, "
        << "booked_at, estimated_delivery_at, "
        << "special_instructions, is_fragile, requires_signature, is_cod, cod_amount, "
        << "created_at, updated_at) VALUES ('"
        << shipment.tracking_number << "', "
        << shipment.sender_id << ", '"
        << shipment.sender_name << "', '"
        << shipment.sender_phone << "', '"
        << shipment.sender_email << "', '"
        << shipment.sender_address << "', '"
        << shipment.sender_city << "', '"
        << shipment.sender_state << "', '"
        << shipment.sender_pincode << "', '"
        << shipment.recipient_name << "', '"
        << shipment.recipient_phone << "', '"
        << shipment.recipient_email << "', '"
        << shipment.recipient_address << "', '"
        << shipment.recipient_city << "', '"
        << shipment.recipient_state << "', '"
        << shipment.recipient_pincode << "', '"
        << shipment.package_type << "', "
        << shipment.weight_grams << ", "
        << shipment.length_cm << ", "
        << shipment.width_cm << ", "
        << shipment.height_cm << ", '"
        << shipment.package_description << "', "
        << shipment.declared_value << ", '"
        << shipment.service_type << "', "
        << shipment.origin_hub_id << ", "
        << shipment.destination_hub_id << ", '"
        << shipment.route_path << "', '"
        << shipment.current_status << "', '"
        << shipment.current_location << "', "
        << shipment.base_cost << ", "
        << shipment.tax_amount << ", "
        << shipment.total_cost << ", '"
        << shipment.booked_at << "', '"
        << shipment.estimated_delivery_at << "', '"
        << shipment.special_instructions << "', "
        << (shipment.is_fragile ? 1 : 0) << ", "
        << (shipment.requires_signature ? 1 : 0) << ", "
        << (shipment.is_cod ? 1 : 0) << ", "
        << shipment.cod_amount << ", '"
        << shipment.created_at << "', '"
        << shipment.updated_at << "')";
    
    return db.execute(sql.str());
}

std::optional<Shipment> ShipmentRepository::find_by_id(int id) {
    auto& db = DatabaseManager::get_instance();
    
    std::ostringstream sql;
    sql << "SELECT * FROM shipments WHERE id = " << id;
    
    auto result = db.query_one(sql.str());
    if (result.empty()) {
        return std::nullopt;
    }
    
    Shipment shipment;
    shipment.id = std::stoi(result["id"]);
    shipment.tracking_number = result["tracking_number"];
    shipment.sender_id = std::stoi(result["sender_id"]);
    shipment.sender_name = result["sender_name"];
    shipment.sender_phone = result["sender_phone"];
    shipment.sender_email = result["sender_email"];
    shipment.sender_address = result["sender_address"];
    shipment.sender_city = result["sender_city"];
    shipment.sender_state = result["sender_state"];
    shipment.sender_pincode = result["sender_pincode"];
    shipment.recipient_name = result["recipient_name"];
    shipment.recipient_phone = result["recipient_phone"];
    shipment.recipient_email = result["recipient_email"];
    shipment.recipient_address = result["recipient_address"];
    shipment.recipient_city = result["recipient_city"];
    shipment.recipient_state = result["recipient_state"];
    shipment.recipient_pincode = result["recipient_pincode"];
    shipment.package_type = result["package_type"];
    shipment.weight_grams = std::stod(result["weight_grams"]);
    shipment.length_cm = result["length_cm"].empty() ? 0 : std::stod(result["length_cm"]);
    shipment.width_cm = result["width_cm"].empty() ? 0 : std::stod(result["width_cm"]);
    shipment.height_cm = result["height_cm"].empty() ? 0 : std::stod(result["height_cm"]);
    shipment.package_description = result["package_description"];
    shipment.declared_value = std::stod(result["declared_value"]);
    shipment.service_type = result["service_type"];
    shipment.origin_hub_id = result["origin_hub_id"].empty() ? 0 : std::stoi(result["origin_hub_id"]);
    shipment.destination_hub_id = result["destination_hub_id"].empty() ? 0 : std::stoi(result["destination_hub_id"]);
    shipment.route_path = result["route_path"];
    shipment.assigned_agent_id = result["assigned_agent_id"].empty() ? 0 : std::stoi(result["assigned_agent_id"]);
    shipment.current_status = result["current_status"];
    shipment.current_location = result["current_location"];
    shipment.base_cost = std::stod(result["base_cost"]);
    shipment.tax_amount = std::stod(result["tax_amount"]);
    shipment.total_cost = std::stod(result["total_cost"]);
    shipment.booked_at = result["booked_at"];
    shipment.pickup_scheduled_at = result["pickup_scheduled_at"];
    shipment.estimated_delivery_at = result["estimated_delivery_at"];
    shipment.actual_delivery_at = result["actual_delivery_at"];
    shipment.special_instructions = result["special_instructions"];
    shipment.is_fragile = result["is_fragile"] == "1";
    shipment.requires_signature = result["requires_signature"] == "1";
    shipment.is_cod = result["is_cod"] == "1";
    shipment.cod_amount = std::stod(result["cod_amount"]);
    shipment.failure_reason = result["failure_reason"];
    shipment.created_at = result["created_at"];
    shipment.updated_at = result["updated_at"];
    
    return shipment;
}

std::optional<Shipment> ShipmentRepository::find_by_tracking_number(const std::string& tracking_number) {
    auto& db = DatabaseManager::get_instance();
    
    std::ostringstream sql;
    sql << "SELECT * FROM shipments WHERE tracking_number = '" << tracking_number << "'";
    
    auto result = db.query_one(sql.str());
    if (result.empty()) {
        return std::nullopt;
    }
    
    Shipment shipment;
    shipment.id = std::stoi(result["id"]);
    shipment.tracking_number = result["tracking_number"];
    shipment.sender_id = std::stoi(result["sender_id"]);
    shipment.sender_name = result["sender_name"];
    shipment.sender_phone = result["sender_phone"];
    shipment.sender_email = result["sender_email"];
    shipment.sender_address = result["sender_address"];
    shipment.sender_city = result["sender_city"];
    shipment.sender_state = result["sender_state"];
    shipment.sender_pincode = result["sender_pincode"];
    shipment.recipient_name = result["recipient_name"];
    shipment.recipient_phone = result["recipient_phone"];
    shipment.recipient_email = result["recipient_email"];
    shipment.recipient_address = result["recipient_address"];
    shipment.recipient_city = result["recipient_city"];
    shipment.recipient_state = result["recipient_state"];
    shipment.recipient_pincode = result["recipient_pincode"];
    shipment.package_type = result["package_type"];
    shipment.weight_grams = std::stod(result["weight_grams"]);
    shipment.length_cm = result["length_cm"].empty() ? 0 : std::stod(result["length_cm"]);
    shipment.width_cm = result["width_cm"].empty() ? 0 : std::stod(result["width_cm"]);
    shipment.height_cm = result["height_cm"].empty() ? 0 : std::stod(result["height_cm"]);
    shipment.package_description = result["package_description"];
    shipment.declared_value = std::stod(result["declared_value"]);
    shipment.service_type = result["service_type"];
    shipment.origin_hub_id = result["origin_hub_id"].empty() ? 0 : std::stoi(result["origin_hub_id"]);
    shipment.destination_hub_id = result["destination_hub_id"].empty() ? 0 : std::stoi(result["destination_hub_id"]);
    shipment.route_path = result["route_path"];
    shipment.assigned_agent_id = result["assigned_agent_id"].empty() ? 0 : std::stoi(result["assigned_agent_id"]);
    shipment.current_status = result["current_status"];
    shipment.current_location = result["current_location"];
    shipment.base_cost = std::stod(result["base_cost"]);
    shipment.tax_amount = std::stod(result["tax_amount"]);
    shipment.total_cost = std::stod(result["total_cost"]);
    shipment.booked_at = result["booked_at"];
    shipment.pickup_scheduled_at = result["pickup_scheduled_at"];
    shipment.estimated_delivery_at = result["estimated_delivery_at"];
    shipment.actual_delivery_at = result["actual_delivery_at"];
    shipment.special_instructions = result["special_instructions"];
    shipment.is_fragile = result["is_fragile"] == "1";
    shipment.requires_signature = result["requires_signature"] == "1";
    shipment.is_cod = result["is_cod"] == "1";
    shipment.cod_amount = std::stod(result["cod_amount"]);
    shipment.failure_reason = result["failure_reason"];
    shipment.created_at = result["created_at"];
    shipment.updated_at = result["updated_at"];
    
    return shipment;
}

bool ShipmentRepository::update_status(int shipment_id, const std::string& status, const std::string& location) {
    auto& db = DatabaseManager::get_instance();
    
    std::ostringstream sql;
    sql << "UPDATE shipments SET current_status = '" << status
        << "', current_location = '" << location
        << "', updated_at = '" << DateTimeUtils::now() << "'";
    
    if (status == "DELIVERED") {
        sql << ", actual_delivery_at = '" << DateTimeUtils::now() << "'";
    }
    
    sql << " WHERE id = " << shipment_id;
    
    return db.execute(sql.str());
}

bool ShipmentRepository::assign_agent(int shipment_id, int agent_id) {
    auto& db = DatabaseManager::get_instance();
    
    std::ostringstream sql;
    sql << "UPDATE shipments SET assigned_agent_id = " << agent_id
        << ", updated_at = '" << DateTimeUtils::now()
        << "' WHERE id = " << shipment_id;
    
    return db.execute(sql.str());
}

std::vector<Shipment> ShipmentRepository::get_by_sender(int sender_id, int limit, int offset) {
    auto& db = DatabaseManager::get_instance();
    
    std::ostringstream sql;
    sql << "SELECT * FROM shipments WHERE sender_id = " << sender_id
        << " ORDER BY booked_at DESC LIMIT " << limit << " OFFSET " << offset;
    
    auto results = db.query(sql.str());
    std::vector<Shipment> shipments;
    
    for (const auto& row : results) {
        Shipment shipment;
        shipment.id = std::stoi(row.at("id"));
        shipment.tracking_number = row.at("tracking_number");
        shipment.sender_id = std::stoi(row.at("sender_id"));
        shipment.sender_name = row.at("sender_name");
        shipment.recipient_name = row.at("recipient_name");
        shipment.recipient_city = row.at("recipient_city");
        shipment.current_status = row.at("current_status");
        shipment.current_location = row.at("current_location");
        shipment.total_cost = std::stod(row.at("total_cost"));
        shipment.booked_at = row.at("booked_at");
        shipment.estimated_delivery_at = row.at("estimated_delivery_at");
        shipments.push_back(shipment);
    }
    
    return shipments;
}

std::vector<Shipment> ShipmentRepository::get_by_agent(int agent_id, int limit, int offset) {
    auto& db = DatabaseManager::get_instance();
    
    std::ostringstream sql;
    sql << "SELECT * FROM shipments WHERE assigned_agent_id = " << agent_id
        << " ORDER BY booked_at DESC LIMIT " << limit << " OFFSET " << offset;
    
    auto results = db.query(sql.str());
    std::vector<Shipment> shipments;
    
    for (const auto& row : results) {
        Shipment shipment;
        shipment.id = std::stoi(row.at("id"));
        shipment.tracking_number = row.at("tracking_number");
        shipment.sender_name = row.at("sender_name");
        shipment.recipient_name = row.at("recipient_name");
        shipment.recipient_city = row.at("recipient_city");
        shipment.recipient_address = row.at("recipient_address");
        shipment.current_status = row.at("current_status");
        shipment.current_location = row.at("current_location");
        shipment.booked_at = row.at("booked_at");
        shipments.push_back(shipment);
    }
    
    return shipments;
}

std::vector<Shipment> ShipmentRepository::get_all(int limit, int offset, const std::string& status_filter) {
    auto& db = DatabaseManager::get_instance();
    
    std::ostringstream sql;
    sql << "SELECT * FROM shipments WHERE 1=1";
    
    if (!status_filter.empty()) {
        sql << " AND current_status = '" << status_filter << "'";
    }
    
    sql << " ORDER BY booked_at DESC LIMIT " << limit << " OFFSET " << offset;
    
    auto results = db.query(sql.str());
    std::vector<Shipment> shipments;
    
    for (const auto& row : results) {
        Shipment shipment;
        shipment.id = std::stoi(row.at("id"));
        shipment.tracking_number = row.at("tracking_number");
        shipment.sender_id = std::stoi(row.at("sender_id"));
        shipment.sender_name = row.at("sender_name");
        shipment.recipient_name = row.at("recipient_name");
        shipment.current_status = row.at("current_status");
        shipment.current_location = row.at("current_location");
        shipment.booked_at = row.at("booked_at");
        shipments.push_back(shipment);
    }
    
    return shipments;
}

int ShipmentRepository::get_count(const std::string& status_filter) {
    auto& db = DatabaseManager::get_instance();
    
    std::ostringstream sql;
    sql << "SELECT COUNT(*) as count FROM shipments WHERE 1=1";
    
    if (!status_filter.empty()) {
        sql << " AND current_status = '" << status_filter << "'";
    }
    
    auto result = db.query_one(sql.str());
    return std::stoi(result["count"]);
}

int ShipmentRepository::get_count_by_sender(int sender_id) {
    auto& db = DatabaseManager::get_instance();
    
    std::ostringstream sql;
    sql << "SELECT COUNT(*) as count FROM shipments WHERE sender_id = " << sender_id;
    
    auto result = db.query_one(sql.str());
    return std::stoi(result["count"]);
}

int ShipmentRepository::get_count_by_agent(int agent_id) {
    auto& db = DatabaseManager::get_instance();
    
    std::ostringstream sql;
    sql << "SELECT COUNT(*) as count FROM shipments WHERE assigned_agent_id = " << agent_id;
    
    auto result = db.query_one(sql.str());
    return std::stoi(result["count"]);
}
