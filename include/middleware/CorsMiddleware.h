// include/middleware/CorsMiddleware.h
#pragma once
#include <crow.h>
#include <string>

class CorsMiddleware {
public:
    // Add CORS headers - origin-aware (reflects the requesting origin)
    static void add_headers(crow::response& res, const std::string& origin);

    // Add CORS headers (uses env fallback for origin)
    static void add_headers(crow::response& res);

    // Handle OPTIONS preflight - reads Origin from request
    static crow::response handle_options(const crow::request& req);

    // Handle OPTIONS preflight (no origin)
    static crow::response handle_options();
};
