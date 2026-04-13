// src/utils/TrackingNumberGenerator.cpp
#include "../../include/utils/TrackingNumberGenerator.h"
#include <chrono>
#include <random>
#include <sstream>
#include <iomanip>

std::atomic<int> TrackingNumberGenerator::counter_{0};

std::string TrackingNumberGenerator::generate() {
    auto now = std::chrono::system_clock::now();
    std::time_t time = std::chrono::system_clock::to_time_t(now);
    std::tm* tm_info = std::localtime(&time);
    
    std::ostringstream date_ss;
    date_ss << std::put_time(tm_info, "%Y%m%d");
    
    const std::string chars = "ABCDEFGHJKLMNPQRSTUVWXYZ23456789";
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, chars.length() - 1);
    
    std::string suffix = "";
    for (int i = 0; i < 7; i++) {
        suffix += chars[dist(gen)];
    }
    
    int count = ++counter_;
    
    return "SWT" + date_ss.str() + suffix + std::to_string(count % 100);
}

bool TrackingNumberGenerator::validate(const std::string& tracking_no) {
    if (tracking_no.length() < 18 || tracking_no.length() > 22) return false;
    if (tracking_no.substr(0, 3) != "SWT") return false;
    return true;
}
