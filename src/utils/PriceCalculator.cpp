// src/utils/PriceCalculator.cpp
#include "../../include/utils/PriceCalculator.h"
#include <cmath>
#include "../../include/config/Config.h"
#include <chrono>
#include <iomanip>
#include <sstream>

double PriceCalculator::calculate_dimensional_weight(double length_cm, double width_cm, double height_cm) {
    if (length_cm <= 0 || width_cm <= 0 || height_cm <= 0) return 0;
    return (length_cm * width_cm * height_cm) / 5.0;
}

double PriceCalculator::get_billable_weight(double actual_weight_grams, double length_cm, double width_cm, double height_cm) {
    double dim_weight = calculate_dimensional_weight(length_cm, width_cm, height_cm);
    return std::max(actual_weight_grams, dim_weight);
}

PriceBreakdown PriceCalculator::calculate(
    double weight_grams,
    double length_cm,
    double width_cm,
    double height_cm,
    double distance_km,
    const std::string& service_type,
    bool is_fragile,
    bool is_cod,
    double cod_amount
) {
    PriceBreakdown breakdown;
    breakdown.service_type = service_type;
    breakdown.weight_grams = weight_grams;
    breakdown.distance_km = distance_km;
    
    breakdown.dimensional_weight_grams = calculate_dimensional_weight(length_cm, width_cm, height_cm);
    breakdown.billable_weight_grams = std::max(weight_grams, breakdown.dimensional_weight_grams);
    
    breakdown.weight_charge = (breakdown.billable_weight_grams / 100.0) * 2.0;
    breakdown.weight_charge = std::max(breakdown.weight_charge, 30.0);
    
    breakdown.distance_charge = distance_km * Config::BASE_PRICE_PER_KM;
    breakdown.distance_charge = std::max(breakdown.distance_charge, 40.0);
    
    double base = breakdown.weight_charge + breakdown.distance_charge;
    
    double multiplier = 1.0;
    if (service_type == "EXPRESS") {
        multiplier = Config::EXPRESS_MULTIPLIER;
        breakdown.service_surcharge = base * (multiplier - 1.0);
    } else if (service_type == "SAME_DAY") {
        multiplier = Config::SAME_DAY_MULTIPLIER;
        breakdown.service_surcharge = base * (multiplier - 1.0);
    }
    
    base *= multiplier;
    
    if (is_fragile) {
        breakdown.fragile_surcharge = 50.0;
        base += breakdown.fragile_surcharge;
    }
    
    if (is_cod && cod_amount > 0) {
        breakdown.cod_charge = std::max(cod_amount * 0.015, 20.0);
        base += breakdown.cod_charge;
    }
    
    breakdown.base_cost = std::round(base * 100.0) / 100.0;
    breakdown.tax_amount = std::round(breakdown.base_cost * Config::GST_RATE * 100.0) / 100.0;
    breakdown.total_cost = breakdown.base_cost + breakdown.tax_amount;
    
    return breakdown;
}

std::string PriceCalculator::calculate_estimated_delivery(const std::string& service_type, double distance_km) {
    int transit_days = 0;
    
    if (service_type == "SAME_DAY") {
        transit_days = 0;
    } else if (service_type == "EXPRESS") {
        if (distance_km < 500) transit_days = 1;
        else if (distance_km < 1500) transit_days = 2;
        else transit_days = 3;
    } else {
        if (distance_km < 300) transit_days = 2;
        else if (distance_km < 800) transit_days = 4;
        else if (distance_km < 1500) transit_days = 5;
        else transit_days = 7;
    }
    
    transit_days += 1;
    
    auto now = std::chrono::system_clock::now();
    auto delivery = now + std::chrono::hours(transit_days * 24);
    std::time_t delivery_time = std::chrono::system_clock::to_time_t(delivery);
    std::tm* tm_info = std::localtime(&delivery_time);
    
    while (tm_info->tm_wday == 0 || tm_info->tm_wday == 6) {
        delivery += std::chrono::hours(24);
        delivery_time = std::chrono::system_clock::to_time_t(delivery);
        tm_info = std::localtime(&delivery_time);
    }
    
    std::ostringstream ss;
    ss << std::put_time(tm_info, "%Y-%m-%dT18:00:00");
    return ss.str();
}
