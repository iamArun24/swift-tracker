// include/services/AnalyticsService.h
#pragma once
#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include "../models/Analytics.h"

class AnalyticsService {
public:
    // Get dashboard stats
    static DashboardStats get_dashboard_stats();
    
    // Get shipment trends
    static std::vector<ShipmentTrend> get_shipment_trends(const std::string& period = "daily", int days = 30);
    
    // Get revenue analytics
    static nlohmann::json get_revenue_analytics(const std::string& period = "monthly", int months = 12);
    
    // Get agent performance comparison
    static std::vector<AgentPerformance> get_agent_performance();
    
    // Get popular routes
    static nlohmann::json get_popular_routes(int limit = 10);
};
