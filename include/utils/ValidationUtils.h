// include/utils/ValidationUtils.h
#pragma once
#include <string>
#include <vector>
#include <nlohmann/json.hpp>

class ValidationUtils {
public:
    // Validate email format
    static bool is_valid_email(const std::string& email);
    
    // Validate phone number (10 digits for India)
    static bool is_valid_phone(const std::string& phone);
    
    // Validate pincode (6 digits for India)
    static bool is_valid_pincode(const std::string& pincode);
    
    // Validate tracking number format
    static bool is_valid_tracking_number(const std::string& tracking_no);
    
    // Sanitize string to prevent SQL injection
    static std::string sanitize(const std::string& input);
    
    // Validate required fields in JSON
    static std::vector<std::string> validate_required(const nlohmann::json& data, const std::vector<std::string>& required_fields);
};
