// include/controllers/TrackingController.h
#pragma once
#include <crow.h>
#include "../services/TrackingService.h"

class TrackingController {
private:
    TrackingService tracking_service_;
    
public:
    // Public tracking endpoint (no auth required)
    crow::response track_public(const crow::request& req, const std::string& tracking_number);
};
