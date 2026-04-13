// include/models/Shipment.h
#pragma once
#include <string>
#include <vector>
#include <optional>
#include <nlohmann/json.hpp>

enum class ShipmentStatus {
    BOOKED,
    PICKED_UP,
    IN_TRANSIT,
    AT_HUB,
    OUT_FOR_DELIVERY,
    DELIVERED,
    FAILED_DELIVERY,
    RETURNED,
    CANCELLED
};

enum class PackageType {
    DOCUMENT,
    PARCEL,
    FRAGILE,
    HEAVY
};

enum class ServiceType {
    STANDARD,   // 5-7 days
    EXPRESS,    // 2-3 days
    SAME_DAY    // Same day (same city only)
};

// String conversion utilities
inline std::string status_to_string(ShipmentStatus s) {
    switch(s) {
        case ShipmentStatus::BOOKED:            return "BOOKED";
        case ShipmentStatus::PICKED_UP:         return "PICKED_UP";
        case ShipmentStatus::IN_TRANSIT:        return "IN_TRANSIT";
        case ShipmentStatus::AT_HUB:            return "AT_HUB";
        case ShipmentStatus::OUT_FOR_DELIVERY:  return "OUT_FOR_DELIVERY";
        case ShipmentStatus::DELIVERED:         return "DELIVERED";
        case ShipmentStatus::FAILED_DELIVERY:   return "FAILED_DELIVERY";
        case ShipmentStatus::RETURNED:          return "RETURNED";
        case ShipmentStatus::CANCELLED:         return "CANCELLED";
        default:                                return "BOOKED";
    }
}

inline ShipmentStatus string_to_status(const std::string& s) {
    if (s == "PICKED_UP")        return ShipmentStatus::PICKED_UP;
    if (s == "IN_TRANSIT")       return ShipmentStatus::IN_TRANSIT;
    if (s == "AT_HUB")           return ShipmentStatus::AT_HUB;
    if (s == "OUT_FOR_DELIVERY") return ShipmentStatus::OUT_FOR_DELIVERY;
    if (s == "DELIVERED")        return ShipmentStatus::DELIVERED;
    if (s == "FAILED_DELIVERY")  return ShipmentStatus::FAILED_DELIVERY;
    if (s == "RETURNED")         return ShipmentStatus::RETURNED;
    if (s == "CANCELLED")        return ShipmentStatus::CANCELLED;
    return ShipmentStatus::BOOKED;
}

inline std::string package_type_to_string(PackageType t) {
    switch(t) {
        case PackageType::DOCUMENT:  return "DOCUMENT";
        case PackageType::PARCEL:    return "PARCEL";
        case PackageType::FRAGILE:   return "FRAGILE";
        case PackageType::HEAVY:     return "HEAVY";
        default:                     return "PARCEL";
    }
}

inline PackageType string_to_package_type(const std::string& t) {
    if (t == "DOCUMENT") return PackageType::DOCUMENT;
    if (t == "FRAGILE")  return PackageType::FRAGILE;
    if (t == "HEAVY")    return PackageType::HEAVY;
    return PackageType::PARCEL;
}

inline std::string service_type_to_string(ServiceType t) {
    switch(t) {
        case ServiceType::STANDARD:  return "STANDARD";
        case ServiceType::EXPRESS:   return "EXPRESS";
        case ServiceType::SAME_DAY:  return "SAME_DAY";
        default:                     return "STANDARD";
    }
}

inline ServiceType string_to_service_type(const std::string& t) {
    if (t == "EXPRESS")   return ServiceType::EXPRESS;
    if (t == "SAME_DAY")  return ServiceType::SAME_DAY;
    return ServiceType::STANDARD;
}

struct Shipment {
    int id = 0;
    std::string tracking_number;
    int sender_id = 0;
    
    // Sender Details
    std::string sender_name;
    std::string sender_phone;
    std::string sender_email;
    std::string sender_address;
    std::string sender_city;
    std::string sender_state;
    std::string sender_pincode;
    
    // Recipient Details
    std::string recipient_name;
    std::string recipient_phone;
    std::string recipient_email;
    std::string recipient_address;
    std::string recipient_city;
    std::string recipient_state;
    std::string recipient_pincode;
    
    // Package Details
    std::string package_type;
    double weight_grams = 0;
    double length_cm = 0;
    double width_cm = 0;
    double height_cm = 0;
    std::string package_description;
    double declared_value = 0;
    
    // Service Details
    std::string service_type;
    
    // Routing
    int origin_hub_id = 0;
    int destination_hub_id = 0;
    std::string route_path;
    
    // Assignment
    int assigned_agent_id = 0;
    
    // Status & Tracking
    std::string current_status;
    std::string current_location;
    
    // Pricing
    double base_cost = 0;
    double tax_amount = 0;
    double total_cost = 0;
    
    // Dates
    std::string booked_at;
    std::string pickup_scheduled_at;
    std::string estimated_delivery_at;
    std::string actual_delivery_at;
    
    // Additional
    std::string special_instructions;
    bool is_fragile = false;
    bool requires_signature = false;
    bool is_cod = false;
    double cod_amount = 0;
    std::string failure_reason;
    
    std::string created_at;
    std::string updated_at;
    
    nlohmann::json to_json() const {
        return {
            {"id", id},
            {"tracking_number", tracking_number},
            {"sender_id", sender_id},
            {"sender_name", sender_name},
            {"sender_phone", sender_phone},
            {"sender_email", sender_email},
            {"sender_address", sender_address},
            {"sender_city", sender_city},
            {"sender_state", sender_state},
            {"sender_pincode", sender_pincode},
            {"recipient_name", recipient_name},
            {"recipient_phone", recipient_phone},
            {"recipient_email", recipient_email},
            {"recipient_address", recipient_address},
            {"recipient_city", recipient_city},
            {"recipient_state", recipient_state},
            {"recipient_pincode", recipient_pincode},
            {"package_type", package_type},
            {"weight_grams", weight_grams},
            {"length_cm", length_cm},
            {"width_cm", width_cm},
            {"height_cm", height_cm},
            {"package_description", package_description},
            {"declared_value", declared_value},
            {"service_type", service_type},
            {"origin_hub_id", origin_hub_id},
            {"destination_hub_id", destination_hub_id},
            {"route_path", route_path},
            {"assigned_agent_id", assigned_agent_id},
            {"current_status", current_status},
            {"current_location", current_location},
            {"base_cost", base_cost},
            {"tax_amount", tax_amount},
            {"total_cost", total_cost},
            {"booked_at", booked_at},
            {"pickup_scheduled_at", pickup_scheduled_at},
            {"estimated_delivery_at", estimated_delivery_at},
            {"actual_delivery_at", actual_delivery_at},
            {"special_instructions", special_instructions},
            {"is_fragile", is_fragile},
            {"requires_signature", requires_signature},
            {"is_cod", is_cod},
            {"cod_amount", cod_amount},
            {"failure_reason", failure_reason},
            {"created_at", created_at},
            {"updated_at", updated_at}
        };
    }
};

// DTO for creating shipment
struct CreateShipmentRequest {
    std::string sender_name;
    std::string sender_phone;
    std::string sender_email;
    std::string sender_address;
    std::string sender_city;
    std::string sender_state;
    std::string sender_pincode;
    
    std::string recipient_name;
    std::string recipient_phone;
    std::string recipient_email;
    std::string recipient_address;
    std::string recipient_city;
    std::string recipient_state;
    std::string recipient_pincode;
    
    std::string package_type;
    double weight_grams;
    double length_cm;
    double width_cm;
    double height_cm;
    std::string package_description;
    double declared_value;
    
    std::string service_type;
    bool is_fragile;
    bool requires_signature;
    bool is_cod;
    double cod_amount;
    std::string special_instructions;
    
    static CreateShipmentRequest from_json(const nlohmann::json& j) {
        CreateShipmentRequest req;
        req.sender_name = j.value("sender_name", "");
        req.sender_phone = j.value("sender_phone", "");
        req.sender_email = j.value("sender_email", "");
        req.sender_address = j.value("sender_address", "");
        req.sender_city = j.value("sender_city", "");
        req.sender_state = j.value("sender_state", "");
        req.sender_pincode = j.value("sender_pincode", "");
        req.recipient_name = j.value("recipient_name", "");
        req.recipient_phone = j.value("recipient_phone", "");
        req.recipient_email = j.value("recipient_email", "");
        req.recipient_address = j.value("recipient_address", "");
        req.recipient_city = j.value("recipient_city", "");
        req.recipient_state = j.value("recipient_state", "");
        req.recipient_pincode = j.value("recipient_pincode", "");
        req.package_type = j.value("package_type", "PARCEL");
        req.weight_grams = j.value("weight_grams", 0.0);
        req.length_cm = j.value("length_cm", 0.0);
        req.width_cm = j.value("width_cm", 0.0);
        req.height_cm = j.value("height_cm", 0.0);
        req.package_description = j.value("package_description", "");
        req.declared_value = j.value("declared_value", 0.0);
        req.service_type = j.value("service_type", "STANDARD");
        req.is_fragile = j.value("is_fragile", false);
        req.requires_signature = j.value("requires_signature", false);
        req.is_cod = j.value("is_cod", false);
        req.cod_amount = j.value("cod_amount", 0.0);
        req.special_instructions = j.value("special_instructions", "");
        return req;
    }
};
