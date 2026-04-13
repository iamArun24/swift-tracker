// src/middleware/CorsMiddleware.cpp
#include "../../include/middleware/CorsMiddleware.h"
#include "../../include/config/Config.h"
#include <string>

// Reflect the requesting origin back so that Netlify preview URLs,
// production URL, and localhost all work without needing to enumerate them.
static std::string resolve_origin(const std::string& request_origin) {
    if (!request_origin.empty()) {
        return request_origin;
    }
    return Config::get_allowed_origin();
}

void CorsMiddleware::add_headers(crow::response& res, const std::string& origin) {
    res.set_header("Access-Control-Allow-Origin", resolve_origin(origin));
    res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS, PATCH");
    res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
    res.set_header("Access-Control-Allow-Credentials", "true");
    res.set_header("Access-Control-Max-Age", "3600");
}

void CorsMiddleware::add_headers(crow::response& res) {
    add_headers(res, "");
}

crow::response CorsMiddleware::handle_options(const crow::request& req) {
    std::string origin = req.get_header_value("Origin");
    crow::response res(204);
    add_headers(res, origin);
    return res;
}

crow::response CorsMiddleware::handle_options() {
    crow::response res(204);
    add_headers(res, "");
    return res;
}
