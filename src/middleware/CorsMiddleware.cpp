// src/middleware/CorsMiddleware.cpp
#include "../../include/middleware/CorsMiddleware.h"
#include "../../include/config/Config.h"

void CorsMiddleware::add_headers(crow::response& res) {
    res.set_header("Access-Control-Allow-Origin", Config::get_allowed_origin());
    res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
    res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
    res.set_header("Access-Control-Allow-Credentials", "true");
    res.set_header("Access-Control-Max-Age", "3600");
}

crow::response CorsMiddleware::handle_options() {
    crow::response res(200);
    add_headers(res);
    return res;
}
