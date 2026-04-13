// include/utils/JsonUtils.h
#pragma once
#include <string>
#include <nlohmann/json.hpp>

class JsonUtils {
public:
    // Pretty print JSON
    static std::string pretty_print(const nlohmann::json& j);
    
    // Create error response JSON
    static nlohmann::json error_response(const std::string& message, int code = 400);
    
    // Create success response JSON
    static nlohmann::json success_response(const std::string& message, const nlohmann::json& data = nullptr);
    
    // Create paginated response JSON
    static nlohmann::json paginated_response(const nlohmann::json& data, int total, int page, int limit);
};
