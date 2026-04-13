// src/services/AnalyticsService.cpp
#include "../../include/services/AnalyticsService.h"
#include "../../include/database/DatabaseManager.h"
#include <sstream>

DashboardStats AnalyticsService::get_dashboard_stats() {
    auto& db = DatabaseManager::get_instance();
    
    DashboardStats stats;
    
    // Get today's date
    auto today = db.query_one("SELECT date('now') as today");
    std::string today_str = today["today"];
    
    // Total shipments today
    auto today_result = db.query_one("SELECT COUNT(*) as count FROM shipments WHERE date(booked_at) = '" + today_str + "'");
    stats.total_shipments_today = std::stoi(today_result["count"]);
    
    // Total shipments this week (simplified)
    auto week_result = db.query_one("SELECT COUNT(*) as count FROM shipments WHERE booked_at >= date('now', '-7 days')");
    stats.total_shipments_week = std::stoi(week_result["count"]);
    
    // Total shipments this month
    auto month_result = db.query_one("SELECT COUNT(*) as count FROM shipments WHERE booked_at >= date('now', '-30 days')");
    stats.total_shipments_month = std::stoi(month_result["count"]);
    
    // Delivered today
    auto delivered_today = db.query_one("SELECT COUNT(*) as count FROM shipments WHERE current_status = 'DELIVERED' AND date(actual_delivery_at) = '" + today_str + "'");
    stats.delivered_today = std::stoi(delivered_today["count"]);
    
    // In transit
    auto in_transit = db.query_one("SELECT COUNT(*) as count FROM shipments WHERE current_status = 'IN_TRANSIT'");
    stats.in_transit = std::stoi(in_transit["count"]);
    
    // Pending pickup
    auto pending = db.query_one("SELECT COUNT(*) as count FROM shipments WHERE current_status = 'BOOKED'");
    stats.pending_pickup = std::stoi(pending["count"]);
    
    // Failed deliveries
    auto failed = db.query_one("SELECT COUNT(*) as count FROM shipments WHERE current_status = 'FAILED_DELIVERY'");
    stats.failed_deliveries = std::stoi(failed["count"]);
    
    // Calculate success rate
    auto total_deliveries = db.query_one("SELECT COUNT(*) as count FROM shipments WHERE current_status IN ('DELIVERED', 'FAILED_DELIVERY')");
    int total = std::stoi(total_deliveries["count"]);
    stats.delivery_success_rate = total > 0 ? (double)stats.delivered_today / total * 100.0 : 0.0;
    
    // Revenue today
    auto revenue_today = db.query_one("SELECT SUM(total_cost) as revenue FROM shipments WHERE date(booked_at) = '" + today_str + "'");
    stats.revenue_today = revenue_today["revenue"].empty() ? 0.0 : std::stod(revenue_today["revenue"]);
    
    // Revenue this month
    auto revenue_month = db.query_one("SELECT SUM(total_cost) as revenue FROM shipments WHERE booked_at >= date('now', '-30 days')");
    stats.revenue_month = revenue_month["revenue"].empty() ? 0.0 : std::stod(revenue_month["revenue"]);
    
    // Active agents
    auto agents = db.query_one("SELECT COUNT(*) as count FROM users WHERE role = 'AGENT' AND is_active = 1");
    stats.active_agents = std::stoi(agents["count"]);
    
    return stats;
}

std::vector<ShipmentTrend> AnalyticsService::get_shipment_trends(const std::string& period, int days) {
    auto& db = DatabaseManager::get_instance();
    
    std::vector<ShipmentTrend> trends;
    
    for (int i = days - 1; i >= 0; i--) {
        std::ostringstream sql;
        sql << "SELECT date('now', '-" << i << " days') as date, "
            << "COUNT(*) as count, "
            << "SUM(total_cost) as revenue "
            << "FROM shipments WHERE date(booked_at) = date('now', '-" << i << " days')";
        
        auto result = db.query_one(sql.str());
        
        ShipmentTrend trend;
        trend.date = result["date"];
        trend.count = result["count"].empty() ? 0 : std::stoi(result["count"]);
        trend.revenue = result["revenue"].empty() ? 0.0 : std::stod(result["revenue"]);
        
        trends.push_back(trend);
    }
    
    return trends;
}

nlohmann::json AnalyticsService::get_revenue_analytics(const std::string& period, int months) {
    auto& db = DatabaseManager::get_instance();
    
    nlohmann::json analytics = nlohmann::json::array();
    
    for (int i = months - 1; i >= 0; i--) {
        std::ostringstream sql;
        sql << "SELECT strftime('%Y-%m', date('now', '-" << i << " months')) as month, "
            << "SUM(total_cost) as revenue, "
            << "COUNT(*) as count "
            << "FROM shipments WHERE strftime('%Y-%m', booked_at) = strftime('%Y-%m', date('now', '-" << i << " months'))";
        
        auto result = db.query_one(sql.str());
        
        if (!result["month"].empty()) {
            analytics.push_back({
                {"month", result["month"]},
                {"revenue", result["revenue"].empty() ? 0.0 : std::stod(result["revenue"])},
                {"count", result["count"].empty() ? 0 : std::stoi(result["count"])}
            });
        }
    }
    
    return analytics;
}

std::vector<AgentPerformance> AnalyticsService::get_agent_performance() {
    auto& db = DatabaseManager::get_instance();
    
    auto agents = db.query("SELECT id, full_name FROM users WHERE role = 'AGENT' AND is_active = 1");
    
    std::vector<AgentPerformance> performance;
    
    for (const auto& agent : agents) {
        int agent_id = std::stoi(agent.at("id"));
        std::string agent_name = agent.at("full_name");
        
        std::ostringstream sql;
        sql << "SELECT COUNT(*) as total, "
            << "SUM(CASE WHEN current_status = 'DELIVERED' THEN 1 ELSE 0 END) as delivered, "
            << "SUM(CASE WHEN current_status = 'FAILED_DELIVERY' THEN 1 ELSE 0 END) as failed "
            << "FROM shipments WHERE assigned_agent_id = " << agent_id;
        
        auto result = db.query_one(sql.str());
        
        AgentPerformance perf;
        perf.agent_id = agent_id;
        perf.agent_name = agent_name;
        perf.total_deliveries = result["total"].empty() ? 0 : std::stoi(result["total"]);
        perf.successful_deliveries = result["delivered"].empty() ? 0 : std::stoi(result["delivered"]);
        perf.failed_deliveries = result["failed"].empty() ? 0 : std::stoi(result["failed"]);
        perf.success_rate = perf.total_deliveries > 0 ? 
            (double)perf.successful_deliveries / perf.total_deliveries * 100.0 : 0.0;
        perf.avg_delivery_time_hours = 24.0; // Simplified
        
        performance.push_back(perf);
    }
    
    return performance;
}

nlohmann::json AnalyticsService::get_popular_routes(int limit) {
    auto& db = DatabaseManager::get_instance();
    
    std::ostringstream sql;
    sql << "SELECT sender_city, recipient_city, COUNT(*) as count "
        << "FROM shipments GROUP BY sender_city, recipient_city "
        << "ORDER BY count DESC LIMIT " << limit;
    
    auto results = db.query(sql.str());
    
    nlohmann::json routes = nlohmann::json::array();
    
    for (const auto& row : results) {
        routes.push_back({
            {"from", row.at("sender_city")},
            {"to", row.at("recipient_city")},
            {"count", std::stoi(row.at("count"))}
        });
    }
    
    return routes;
}
