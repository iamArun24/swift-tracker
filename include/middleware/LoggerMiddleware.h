// include/middleware/LoggerMiddleware.h
#pragma once
#include <crow.h>
#include <string>

class LoggerMiddleware {
public:
    // Log request
    static void log_request(const crow::request& req);
    
    // Log response
    static void log_response(const crow::response& res, int status_code);
    
    // Get client IP
    static std::string get_client_ip(const crow::request& req);
};
