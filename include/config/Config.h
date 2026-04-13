// include/config/Config.h
#pragma once
#include <string>
#include <cstdlib>

class Config {
public:
    // Server settings
    static constexpr int SERVER_PORT = 8080;
    static constexpr int SERVER_THREADS = 4;
    
    // Database
    static constexpr const char* DB_PATH = "./swifttrack.db";
    
    // JWT settings
    static constexpr const char* JWT_SECRET = "swifttrack_jwt_secret_key_2024_very_long_and_secure";
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
    static constexpr const char* ALLOWED_ORIGIN = "http://localhost:5173";
    
    // Get environment variable with fallback
    static std::string get_env(const std::string& key, const std::string& default_val) {
        const char* val = std::getenv(key.c_str());
        return val ? std::string(val) : default_val;
    }
};
