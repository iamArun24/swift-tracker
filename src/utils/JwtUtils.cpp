// src/utils/JwtUtils.cpp
#include "../../include/utils/JwtUtils.h"
#include <sstream>
#include <iomanip>
#include <chrono>
#include <vector>

std::string JwtUtils::secret_ = "default_secret";

static std::string sign_data(const std::string& data, const std::string& secret) {
    // Lightweight fallback signature when OpenSSL isn't available.
    auto digest = std::hash<std::string>{}(secret + ":" + data);
    std::ostringstream oss;
    oss << std::hex << digest;
    return oss.str();
}

void JwtUtils::set_secret(const std::string& secret) {
    secret_ = secret;
}

// Base64 URL encoding
static std::string base64_url_encode(const std::string& input) {
    static const std::string chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";

    std::string out;
    int val = 0;
    int valb = -6;
    for (unsigned char c : input) {
        val = (val << 8) + c;
        valb += 8;
        while (valb >= 0) {
            out.push_back(chars[(val >> valb) & 0x3F]);
            valb -= 6;
        }
    }
    if (valb > -6) {
        out.push_back(chars[((val << 8) >> (valb + 8)) & 0x3F]);
    }
    return out;
}

// Base64 URL decoding
static std::string base64_url_decode(const std::string& input) {
    static std::vector<int> table(256, -1);
    static bool initialized = false;
    if (!initialized) {
        const std::string chars =
            "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";
        for (int i = 0; i < 64; i++) {
            table[static_cast<unsigned char>(chars[i])] = i;
        }
        initialized = true;
    }

    std::string out;
    int val = 0;
    int valb = -8;
    for (unsigned char c : input) {
        int d = table[c];
        if (d == -1) {
            break;
        }
        val = (val << 6) + d;
        valb += 6;
        if (valb >= 0) {
            out.push_back(static_cast<char>((val >> valb) & 0xFF));
            valb -= 8;
        }
    }
    return out;
}

std::string JwtUtils::generate(const JwtPayload& payload) {
    // Header
    nlohmann::json header = {
        {"alg", "HS256"},
        {"typ", "JWT"}
    };
    
    std::string header_encoded = base64_url_encode(header.dump());
    std::string payload_encoded = base64_url_encode(payload.to_json().dump());
    
    std::string data = header_encoded + "." + payload_encoded;
    
    std::string signature = sign_data(data, secret_);
    std::string signature_encoded = base64_url_encode(signature);
    
    return data + "." + signature_encoded;
}

bool JwtUtils::verify(const std::string& token, JwtPayload& payload) {
    size_t first_dot = token.find('.');
    size_t second_dot = token.find('.', first_dot + 1);
    
    if (first_dot == std::string::npos || second_dot == std::string::npos) {
        return false;
    }
    
    std::string header_encoded = token.substr(0, first_dot);
    std::string payload_encoded = token.substr(first_dot + 1, second_dot - first_dot - 1);
    std::string signature_encoded = token.substr(second_dot + 1);
    
    std::string data = header_encoded + "." + payload_encoded;
    
    std::string signature = sign_data(data, secret_);
    std::string signature_encoded_calc = base64_url_encode(signature);
    
    if (signature_encoded != signature_encoded_calc) {
        return false;
    }
    
    // Decode payload
    try {
        std::string payload_str = base64_url_decode(payload_encoded);
        nlohmann::json payload_json = nlohmann::json::parse(payload_str);
        payload = JwtPayload::from_json(payload_json);
        
        // Check expiry
        auto now = std::chrono::system_clock::now();
        auto now_ts = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
        
        if (payload.exp < now_ts) {
            return false;
        }
        
        return true;
    } catch (...) {
        return false;
    }
}

JwtPayload JwtUtils::decode(const std::string& token) {
    size_t first_dot = token.find('.');
    size_t second_dot = token.find('.', first_dot + 1);
    
    if (first_dot == std::string::npos || second_dot == std::string::npos) {
        return JwtPayload{};
    }
    
    std::string payload_encoded = token.substr(first_dot + 1, second_dot - first_dot - 1);
    
    try {
        std::string payload_str = base64_url_decode(payload_encoded);
        nlohmann::json payload_json = nlohmann::json::parse(payload_str);
        return JwtPayload::from_json(payload_json);
    } catch (...) {
        return JwtPayload{};
    }
}

long JwtUtils::get_expiry(int hours_from_now) {
    auto now = std::chrono::system_clock::now();
    auto expiry = now + std::chrono::hours(hours_from_now);
    return std::chrono::duration_cast<std::chrono::seconds>(expiry.time_since_epoch()).count();
}
