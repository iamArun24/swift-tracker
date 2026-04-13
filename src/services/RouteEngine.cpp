// src/services/RouteEngine.cpp
#include "../../include/services/RouteEngine.h"
#include "../../include/database/DatabaseManager.h"
#include "../../include/database/HubRepository.h"
#include <unordered_map>
#include <queue>
#include <limits>
#include <algorithm>
#include <sstream>
#include <cmath>

static std::unordered_map<int, HubNode> nodes;
static std::unordered_map<int, std::vector<std::tuple<int, double, int>>> adj;
static bool graph_loaded = false;

void RouteEngine::initialize() {
    if (graph_loaded) return;
    
    auto hubs = HubRepository::get_all(true);
    
    for (const auto& hub : hubs) {
        HubNode node;
        node.id = hub.id;
        node.city_name = hub.city_name;
        node.hub_code = hub.hub_code;
        node.latitude = hub.latitude;
        node.longitude = hub.longitude;
        nodes[hub.id] = node;
    }
    
    // Load routes from database
    auto& db = DatabaseManager::get_instance();
    auto routes = db.query("SELECT from_hub_id, to_hub_id, distance_km, transit_hours FROM routes WHERE is_active = 1");
    
    for (const auto& row : routes) {
        int from_id = std::stoi(row.at("from_hub_id"));
        int to_id = std::stoi(row.at("to_hub_id"));
        double distance = std::stod(row.at("distance_km"));
        int hours = std::stoi(row.at("transit_hours"));
        
        adj[from_id].push_back({to_id, distance, hours});
        adj[to_id].push_back({from_id, distance, hours}); // Bidirectional
    }
    
    graph_loaded = true;
}

std::optional<HubNode> RouteEngine::find_hub_by_city(const std::string& city_name) {
    if (!graph_loaded) initialize();
    
    std::string lower_city = city_name;
    std::transform(lower_city.begin(), lower_city.end(), lower_city.begin(), ::tolower);
    
    for (const auto& [id, node] : nodes) {
        std::string lower_node = node.city_name;
        std::transform(lower_node.begin(), lower_node.end(), lower_node.begin(), ::tolower);
        
        if (lower_node == lower_city || 
            lower_node.find(lower_city) != std::string::npos ||
            lower_city.find(lower_node) != std::string::npos) {
            return node;
        }
    }
    
    return std::nullopt;
}

RouteResult RouteEngine::find_optimal_route(const std::string& from_city, const std::string& to_city) {
    if (!graph_loaded) initialize();
    
    RouteResult result;
    
    auto from_hub = find_hub_by_city(from_city);
    auto to_hub = find_hub_by_city(to_city);
    
    if (!from_hub || !to_hub) {
        result.found = false;
        return result;
    }
    
    int from_id = from_hub->id;
    int to_id = to_hub->id;
    
    if (from_id == to_id) {
        result.found = true;
        result.hub_ids = {from_id};
        result.hub_names = {from_hub->city_name};
        result.total_distance_km = 0;
        result.total_transit_hours = 0;
        result.route_path_json = "[" + std::to_string(from_id) + "]";
        return result;
    }
    
    // Dijkstra's algorithm
    using PQElement = std::pair<double, int>;
    std::priority_queue<PQElement, std::vector<PQElement>, std::greater<PQElement>> pq;
    
    std::unordered_map<int, double> dist;
    std::unordered_map<int, int> prev;
    std::unordered_map<int, int> prev_transit;
    
    for (const auto& [id, _] : nodes) {
        dist[id] = std::numeric_limits<double>::infinity();
    }
    
    dist[from_id] = 0;
    pq.push({0.0, from_id});
    
    while (!pq.empty()) {
        auto [d, u] = pq.top();
        pq.pop();
        
        if (d > dist[u]) continue;
        if (u == to_id) break;
        
        if (adj.find(u) != adj.end()) {
            for (const auto& [v, edge_dist, edge_transit] : adj[u]) {
                double new_dist = dist[u] + edge_dist;
                if (new_dist < dist[v]) {
                    dist[v] = new_dist;
                    prev[v] = u;
                    prev_transit[v] = edge_transit;
                    pq.push({new_dist, v});
                }
            }
        }
    }
    
    if (dist[to_id] == std::numeric_limits<double>::infinity()) {
        result.found = false;
        return result;
    }
    
    std::vector<int> path;
    int transit_total = 0;
    
    for (int at = to_id; at != from_id; at = prev[at]) {
        path.push_back(at);
        transit_total += prev_transit[at];
    }
    path.push_back(from_id);
    std::reverse(path.begin(), path.end());
    
    result.found = true;
    result.hub_ids = path;
    result.total_distance_km = dist[to_id];
    result.total_transit_hours = transit_total;
    
    nlohmann::json path_json = nlohmann::json::array();
    for (int hub_id : path) {
        if (nodes.find(hub_id) != nodes.end()) {
            result.hub_names.push_back(nodes[hub_id].city_name);
            path_json.push_back({
                {"hub_id", hub_id},
                {"city", nodes[hub_id].city_name},
                {"hub_code", nodes[hub_id].hub_code},
                {"lat", nodes[hub_id].latitude},
                {"lng", nodes[hub_id].longitude}
            });
        }
    }
    
    result.route_path_json = path_json.dump();
    return result;
}

nlohmann::json RouteEngine::get_all_hubs_json() {
    if (!graph_loaded) initialize();
    
    nlohmann::json hubs = nlohmann::json::array();
    for (const auto& [id, node] : nodes) {
        hubs.push_back({
            {"id", id},
            {"city_name", node.city_name},
            {"hub_code", node.hub_code},
            {"latitude", node.latitude},
            {"longitude", node.longitude}
        });
    }
    return hubs;
}

void RouteEngine::reload() {
    nodes.clear();
    adj.clear();
    graph_loaded = false;
    initialize();
}
