// include/config/Config.h
#pragma once
#include <string>
#include <cstdlib>

class Config {
public:
    // Server settings
    static int get_port() {
        const char* port = std::getenv("PORT");
        return port ? std::stoi(port) : 8080;
    }
    static constexpr int SERVER_THREADS = 4;
    
    // Database (For production, use PostgreSQL via RENDER_DB_URL)
    static std::string get_db_path() {
        const char* db_url = std::getenv("RENDER_DB_URL");
        if (db_url) return std::string(db_url);
        const char* db_path = std::getenv("DB_PATH");
        return db_path ? std::string(db_path) : "./swifttrack.db";
    }
    
    // JWT settings
    static std::string get_jwt_secret() {
        const char* secret = std::getenv("JWT_SECRET");
        return secret ? std::string(secret) : "swifttrack_jwt_secret_key_2024_very_long_and_secure";
    }
    static constexpr int JWT_EXPIRY_HOURS = 24;
    
    // Pricing constants
    static constexpr double BASE_PRICE_PER_KM = 0.5;   // Rs per km
    static constexpr double BASE_PRICE_PER_GRAM = 0.02; // Rs per gram
    static constexpr double GST_RATE = 0.18;             // 18% GST
    
    // Service multipliers
    static constexpr double STANDARD_MULTIPLIER = 1.0;
    static constexpr double EXPRESS_MULTIPLIER = 1.75;
    static constexpr double SAME_DAY_MULTIPLIER = 3.0;
    
    // Rate limiting
    static constexpr int RATE_LIMIT_REQUESTS = 100;  // per minute
    
    // App info
    static constexpr const char* APP_NAME = "SwiftTrack";
    static constexpr const char* APP_VERSION = "1.0.0";
    
    // CORS
    static std::string get_allowed_origin() {
        const char* origin = std::getenv("ALLOWED_ORIGIN");
        return origin ? std::string(origin) : "https://swift-tracker.netlify.app";
    }
    
    // Get environment variable with fallback
    static std::string get_env(const std::string& key, const std::string& default_val) {
        const char* val = std::getenv(key.c_str());
        return val ? std::string(val) : default_val;
    }
};
