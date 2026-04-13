// src/utils/JsonUtils.cpp
#include "../../include/utils/JsonUtils.h"

std::string JsonUtils::pretty_print(const nlohmann::json& j) {
    return j.dump(2);
}

nlohmann::json JsonUtils::error_response(const std::string& message, int code) {
    return {
        {"error", message},
        {"code", code}
    };
}

nlohmann::json JsonUtils::success_response(const std::string& message, const nlohmann::json& data) {
    nlohmann::json response = {
        {"message", message}
    };
    
    if (!data.is_null()) {
        response["data"] = data;
    }
    
    return response;
}

nlohmann::json JsonUtils::paginated_response(const nlohmann::json& data, int total, int page, int limit) {
    int total_pages = (total + limit - 1) / limit;
    
    return {
        {"data", data},
        {"pagination", {
            {"total", total},
            {"page", page},
            {"limit", limit},
            {"total_pages", total_pages},
            {"has_next", page < total_pages},
            {"has_prev", page > 1}
        }}
    };
}
