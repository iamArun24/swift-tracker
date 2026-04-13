// include/models/Analytics.h
#pragma once
#include <string>
#include <map>
#include <nlohmann/json.hpp>

struct DashboardStats {
    int total_shipments_today = 0;
    int total_shipments_week = 0;
    int total_shipments_month = 0;
    int delivered_today = 0;
    int in_transit = 0;
    int pending_pickup = 0;
    int failed_deliveries = 0;
    double delivery_success_rate = 0.0;
    double revenue_today = 0.0;
    double revenue_month = 0.0;
    int active_agents = 0;
    
    nlohmann::json to_json() const {
        return {
            {"total_shipments_today", total_shipments_today},
            {"total_shipments_week", total_shipments_week},
            {"total_shipments_month", total_shipments_month},
            {"delivered_today", delivered_today},
            {"in_transit", in_transit},
            {"pending_pickup", pending_pickup},
            {"failed_deliveries", failed_deliveries},
            {"delivery_success_rate", delivery_success_rate},
            {"revenue_today", revenue_today},
            {"revenue_month", revenue_month},
            {"active_agents", active_agents}
        };
    }
};

struct ShipmentTrend {
    std::string date;
    int count = 0;
    double revenue = 0.0;
    
    nlohmann::json to_json() const {
        return {
            {"date", date},
            {"count", count},
            {"revenue", revenue}
        };
    }
};

struct AgentPerformance {
    int agent_id = 0;
    std::string agent_name;
    int total_deliveries = 0;
    int successful_deliveries = 0;
    int failed_deliveries = 0;
    double success_rate = 0.0;
    double avg_delivery_time_hours = 0.0;
    
    nlohmann::json to_json() const {
        return {
            {"agent_id", agent_id},
            {"agent_name", agent_name},
            {"total_deliveries", total_deliveries},
            {"successful_deliveries", successful_deliveries},
            {"failed_deliveries", failed_deliveries},
            {"success_rate", success_rate},
            {"avg_delivery_time_hours", avg_delivery_time_hours}
        };
    }
};
