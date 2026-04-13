// src/utils/DateTimeUtils.cpp
#include "../../include/utils/DateTimeUtils.h"
#include <chrono>
#include <iomanip>
#include <sstream>
#include <random>
#include <sstream>

std::string DateTimeUtils::now() {
    auto now = std::chrono::system_clock::now();
    std::time_t time = std::chrono::system_clock::to_time_t(now);
    std::tm* tm_info = std::gmtime(&time);
    
    std::ostringstream ss;
    ss << std::put_time(tm_info, "%Y-%m-%dT%H:%M:%S");
    return ss.str();
}

std::string DateTimeUtils::now_local() {
    auto now = std::chrono::system_clock::now();
    std::time_t time = std::chrono::system_clock::to_time_t(now);
    std::tm* tm_info = std::localtime(&time);
    
    std::ostringstream ss;
    ss << std::put_time(tm_info, "%Y-%m-%dT%H:%M:%S");
    return ss.str();
}

std::string DateTimeUtils::add_hours(const std::string& datetime, int hours) {
    std::tm tm = {};
    std::istringstream ss(datetime);
    ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%S");
    
    auto time = std::mktime(&tm);
    time += hours * 3600;
    
    std::tm* tm_info = std::localtime(&time);
    std::ostringstream oss;
    oss << std::put_time(tm_info, "%Y-%m-%dT%H:%M:%S");
    return oss.str();
}

std::string DateTimeUtils::add_days(const std::string& datetime, int days) {
    std::tm tm = {};
    std::istringstream ss(datetime);
    ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%S");
    
    auto time = std::mktime(&tm);
    time += days * 24 * 3600;
    
    std::tm* tm_info = std::localtime(&time);
    std::ostringstream oss;
    oss << std::put_time(tm_info, "%Y-%m-%dT%H:%M:%S");
    return oss.str();
}

std::string DateTimeUtils::format_display(const std::string& datetime) {
    std::tm tm = {};
    std::istringstream ss(datetime);
    ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%S");
    
    std::ostringstream oss;
    oss << std::put_time(&tm, "%d %b %Y, %I:%M %p");
    return oss.str();
}

long DateTimeUtils::to_timestamp(const std::string& datetime) {
    std::tm tm = {};
    std::istringstream ss(datetime);
    ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%S");
    return std::mktime(&tm);
}

std::string DateTimeUtils::generate_uuid() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);
    
    std::ostringstream ss;
    ss << std::hex;
    
    for (int i = 0; i < 8; i++) ss << dis(gen);
    ss << "-";
    for (int i = 0; i < 4; i++) ss << dis(gen);
    ss << "-4";
    for (int i = 0; i < 3; i++) ss << dis(gen);
    ss << "-";
    ss << std::hex << (dis(gen) & 0x3 | 0x8);
    for (int i = 0; i < 3; i++) ss << dis(gen);
    ss << "-";
    for (int i = 0; i < 12; i++) ss << dis(gen);
    
    return ss.str();
}
