// src/middleware/LoggerMiddleware.cpp
#include "../../include/middleware/LoggerMiddleware.h"
#include <iostream>
#include <ctime>

void LoggerMiddleware::log_request(const crow::request& req) {
    std::string method = crow::method_name(req.method);
    std::string url = req.url;
    std::string ip = get_client_ip(req);
    
    std::time_t now = std::time(nullptr);
    char timestamp[100];
    std::strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
    
    std::cout << "[" << timestamp << "] " << method << " " << url << " from " << ip << std::endl;
}

void LoggerMiddleware::log_response(const crow::response& res, int status_code) {
    std::cout << "Response: " << status_code << std::endl;
}

std::string LoggerMiddleware::get_client_ip(const crow::request& req) {
    auto ip = req.get_header_value("X-Forwarded-For");
    if (!ip.empty()) {
        return ip;
    }
    
    ip = req.get_header_value("X-Real-IP");
    if (!ip.empty()) {
        return ip;
    }
    
    return "unknown";
}
