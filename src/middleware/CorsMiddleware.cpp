// src/middleware/CorsMiddleware.cpp
#include "../../include/middleware/CorsMiddleware.h"

void CorsMiddleware::add_headers(crow::response& res, const std::string& origin) {
    res.set_header("Access-Control-Allow-Origin", "*");
    res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS, PATCH");
    res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization, X-Requested-With");
    res.set_header("Access-Control-Max-Age", "86400");
}

void CorsMiddleware::add_headers(crow::response& res) {
    add_headers(res, "");
}

crow::response CorsMiddleware::handle_options(const crow::request& req) {
    crow::response res(204);
    add_headers(res, "");
    return res;
}

crow::response CorsMiddleware::handle_options() {
    crow::response res(204);
    add_headers(res, "");
    return res;
}
