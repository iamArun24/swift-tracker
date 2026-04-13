// include/middleware/CorsMiddleware.h
#pragma once
#include <crow.h>

class CorsMiddleware {
public:
    // Add CORS headers to response
    static void add_headers(crow::response& res);
    
    // Handle OPTIONS preflight request
    static crow::response handle_options();
};
