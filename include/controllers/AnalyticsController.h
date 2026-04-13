// include/controllers/AnalyticsController.h
#pragma once
#include <crow.h>
#include "../services/AnalyticsService.h"

class AnalyticsController {
private:
    AnalyticsService analytics_service_;
    
public:
    // Get dashboard stats (requires auth: ADMIN)
    crow::response get_dashboard(const crow::request& req);
    
    // Get shipment trends (requires auth: ADMIN)
    crow::response get_shipment_trends(const crow::request& req);
    
    // Get revenue analytics (requires auth: ADMIN)
    crow::response get_revenue(const crow::request& req);
    
    // Get agent performance (requires auth: ADMIN)
    crow::response get_agent_performance(const crow::request& req);
    
    // Get popular routes (requires auth: ADMIN)
    crow::response get_popular_routes(const crow::request& req);
};
