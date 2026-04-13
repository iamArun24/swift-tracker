// include/utils/PriceCalculator.h
#pragma once
#include <string>
#include <nlohmann/json.hpp>

struct PriceBreakdown {
    double weight_charge = 0;
    double distance_charge = 0;
    double dimensional_weight_charge = 0;
    double service_surcharge = 0;
    double fragile_surcharge = 0;
    double cod_charge = 0;
    double base_cost = 0;
    double tax_amount = 0;
    double total_cost = 0;
    
    std::string service_type;
    double weight_grams = 0;
    double distance_km = 0;
    double dimensional_weight_grams = 0;
    double billable_weight_grams = 0;
    
    nlohmann::json to_json() const {
        return {
            {"weight_charge", weight_charge},
            {"distance_charge", distance_charge},
            {"dimensional_weight_charge", dimensional_weight_charge},
            {"service_surcharge", service_surcharge},
            {"fragile_surcharge", fragile_surcharge},
            {"cod_charge", cod_charge},
            {"base_cost", base_cost},
            {"tax_amount", tax_amount},
            {"total_cost", total_cost},
            {"service_type", service_type},
            {"weight_grams", weight_grams},
            {"distance_km", distance_km},
            {"dimensional_weight_grams", dimensional_weight_grams},
            {"billable_weight_grams", billable_weight_grams}
        };
    }
};

class PriceCalculator {
public:
    // Calculate dimensional weight (volumetric weight)
    static double calculate_dimensional_weight(double length_cm, double width_cm, double height_cm);
    
    // Get billable weight (max of actual vs dimensional)
    static double get_billable_weight(double actual_weight_grams, double length_cm, double width_cm, double height_cm);
    
    // Main price calculation function
    static PriceBreakdown calculate(
        double weight_grams,
        double length_cm,
        double width_cm,
        double height_cm,
        double distance_km,
        const std::string& service_type,
        bool is_fragile = false,
        bool is_cod = false,
        double cod_amount = 0
    );
    
    // Calculate estimated delivery date (returns ISO date string)
    static std::string calculate_estimated_delivery(const std::string& service_type, double distance_km);
};
