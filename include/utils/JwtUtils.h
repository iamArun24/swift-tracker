// include/utils/JwtUtils.h
#pragma once
#include <string>
#include <map>
#include <nlohmann/json.hpp>

struct JwtPayload {
    int user_id;
    std::string email;
    std::string role;
    long exp; // expiry timestamp
    
    nlohmann::json to_json() const {
        return {
            {"user_id", user_id},
            {"email", email},
            {"role", role},
            {"exp", exp}
        };
    }
    
    static JwtPayload from_json(const nlohmann::json& j) {
        JwtPayload payload;
        payload.user_id = j.value("user_id", 0);
        payload.email = j.value("email", "");
        payload.role = j.value("role", "");
        payload.exp = j.value("exp", 0L);
        return payload;
    }
};

class JwtUtils {
private:
    static std::string secret_;
    
public:
    static void set_secret(const std::string& secret);
    
    // Generate JWT token
    static std::string generate(const JwtPayload& payload);
    
    // Verify and decode JWT token
    static bool verify(const std::string& token, JwtPayload& payload);
    
    // Decode without verification (for debugging)
    static JwtPayload decode(const std::string& token);
    
    // Get expiry timestamp (seconds since epoch)
    static long get_expiry(int hours_from_now = 24);
};
