// src/services/ShipmentService.cpp
#include "../../include/services/ShipmentService.h"
#include "../../include/database/ShipmentRepository.h"
#include "../../include/database/TrackingRepository.h"
#include "../../include/database/HubRepository.h"
#include "../../include/services/RouteEngine.h"
#include "../../include/utils/TrackingNumberGenerator.h"
#include "../../include/utils/PriceCalculator.h"
#include "../../include/utils/DateTimeUtils.h"

std::optional<Shipment> ShipmentService::create_shipment(int sender_id, const CreateShipmentRequest& request) {
    // Generate tracking number
    std::string tracking_number = TrackingNumberGenerator::generate();
    
    // Find origin and destination hubs
    auto origin_hub = RouteEngine::find_hub_by_city(request.sender_city);
    auto dest_hub = RouteEngine::find_hub_by_city(request.recipient_city);
    
    int origin_hub_id = origin_hub.has_value() ? origin_hub->id : 0;
    int dest_hub_id = dest_hub.has_value() ? dest_hub->id : 0;
    
    // Calculate route
    RouteResult route = RouteEngine::find_optimal_route(request.sender_city, request.recipient_city);
    
    // Calculate price
    PriceBreakdown price = PriceCalculator::calculate(
        request.weight_grams,
        request.length_cm,
        request.width_cm,
        request.height_cm,
        route.total_distance_km,
        request.service_type,
        request.is_fragile,
        request.is_cod,
        request.cod_amount
    );
    
    // Calculate estimated delivery
    std::string estimated_delivery = PriceCalculator::calculate_estimated_delivery(
        request.service_type,
        route.total_distance_km
    );
    
    // Create shipment
    Shipment shipment;
    shipment.tracking_number = tracking_number;
    shipment.sender_id = sender_id;
    shipment.sender_name = request.sender_name;
    shipment.sender_phone = request.sender_phone;
    shipment.sender_email = request.sender_email;
    shipment.sender_address = request.sender_address;
    shipment.sender_city = request.sender_city;
    shipment.sender_state = request.sender_state;
    shipment.sender_pincode = request.sender_pincode;
    shipment.recipient_name = request.recipient_name;
    shipment.recipient_phone = request.recipient_phone;
    shipment.recipient_email = request.recipient_email;
    shipment.recipient_address = request.recipient_address;
    shipment.recipient_city = request.recipient_city;
    shipment.recipient_state = request.recipient_state;
    shipment.recipient_pincode = request.recipient_pincode;
    shipment.package_type = request.package_type;
    shipment.weight_grams = request.weight_grams;
    shipment.length_cm = request.length_cm;
    shipment.width_cm = request.width_cm;
    shipment.height_cm = request.height_cm;
    shipment.package_description = request.package_description;
    shipment.declared_value = request.declared_value;
    shipment.service_type = request.service_type;
    shipment.origin_hub_id = origin_hub_id;
    shipment.destination_hub_id = dest_hub_id;
    shipment.route_path = route.route_path_json;
    shipment.current_status = status_to_string(ShipmentStatus::BOOKED);
    shipment.current_location = request.sender_city;
    shipment.base_cost = price.base_cost;
    shipment.tax_amount = price.tax_amount;
    shipment.total_cost = price.total_cost;
    shipment.booked_at = DateTimeUtils::now();
    shipment.estimated_delivery_at = estimated_delivery;
    shipment.special_instructions = request.special_instructions;
    shipment.is_fragile = request.is_fragile;
    shipment.requires_signature = request.requires_signature;
    shipment.is_cod = request.is_cod;
    shipment.cod_amount = request.cod_amount;
    shipment.created_at = DateTimeUtils::now();
    shipment.updated_at = DateTimeUtils::now();
    
    if (!ShipmentRepository::create(shipment)) {
        return std::nullopt;
    }
    
    // Get the created shipment with ID
    auto created = ShipmentRepository::find_by_tracking_number(tracking_number);
    
    // Create initial tracking event
    if (created) {
        TrackingRepository::create({
            0,
            created->id,
            "BOOKED",
            request.sender_city,
            0,
            0,
            "Shipment booked successfully. Pickup scheduled.",
            0,
            true,
            DateTimeUtils::now()
        });
    }
    
    return created;
}

std::optional<Shipment> ShipmentService::get_shipment(int shipment_id) {
    return ShipmentRepository::find_by_id(shipment_id);
}

std::optional<Shipment> ShipmentService::get_by_tracking_number(const std::string& tracking_number) {
    return ShipmentRepository::find_by_tracking_number(tracking_number);
}

std::vector<Shipment> ShipmentService::get_sender_shipments(int sender_id, int limit, int offset) {
    return ShipmentRepository::get_by_sender(sender_id, limit, offset);
}

PriceBreakdown ShipmentService::calculate_price_estimate(const CreateShipmentRequest& request, double distance_km) {
    return PriceCalculator::calculate(
        request.weight_grams,
        request.length_cm,
        request.width_cm,
        request.height_cm,
        distance_km,
        request.service_type,
        request.is_fragile,
        request.is_cod,
        request.cod_amount
    );
}

bool ShipmentService::cancel_shipment(int shipment_id) {
    auto shipment = ShipmentRepository::find_by_id(shipment_id);
    if (!shipment) {
        return false;
    }
    
    if (shipment->current_status != "BOOKED") {
        return false;
    }
    
    return ShipmentRepository::update_status(shipment_id, "CANCELLED", shipment->current_location);
}

bool ShipmentService::update_status(int shipment_id, const std::string& status, const std::string& location, int updated_by) {
    return ShipmentRepository::update_status(shipment_id, status, location);
}
