// src/controllers/TrackingController.cpp
#include "../../include/controllers/TrackingController.h"
#include "../../include/utils/JsonUtils.h"
#include <nlohmann/json.hpp>

crow::response TrackingController::track_public(const crow::request& req, const std::string& tracking_number) {
    // No authentication required for public tracking
    auto result = tracking_service_.get_public_tracking(tracking_number);
    
    if (result.contains("error")) {
        nlohmann::json err = JsonUtils::error_response(result["error"], 404);
        return crow::response(404, err.dump());
    }
    
    nlohmann::json response = JsonUtils::success_response("", result);
    return crow::response(200, response.dump());
}
