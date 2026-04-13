// src/utils/ValidationUtils.cpp
#include "../../include/utils/ValidationUtils.h"
#include "../../include/utils/TrackingNumberGenerator.h"
#include <regex>
#include <algorithm>

bool ValidationUtils::is_valid_email(const std::string& email) {
    std::regex pattern(R"([a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,})");
    return std::regex_match(email, pattern);
}

bool ValidationUtils::is_valid_phone(const std::string& phone) {
    std::regex pattern(R"([6-9]\d{9})");
    return std::regex_match(phone, pattern);
}

bool ValidationUtils::is_valid_pincode(const std::string& pincode) {
    std::regex pattern(R"(\d{6})");
    return std::regex_match(pincode, pattern);
}

bool ValidationUtils::is_valid_tracking_number(const std::string& tracking_no) {
    return TrackingNumberGenerator::validate(tracking_no);
}

std::string ValidationUtils::sanitize(const std::string& input) {
    std::string result = input;
    std::string dangerous_chars = "'\"\\;--";
    
    for (char c : dangerous_chars) {
        result.erase(std::remove(result.begin(), result.end(), c), result.end());
    }
    
    return result;
}

std::vector<std::string> ValidationUtils::validate_required(const nlohmann::json& data, const std::vector<std::string>& required_fields) {
    std::vector<std::string> missing;
    
    for (const auto& field : required_fields) {
        if (!data.contains(field) || data[field].is_null() || data[field].get<std::string>().empty()) {
            missing.push_back(field);
        }
    }
    
    return missing;
}
