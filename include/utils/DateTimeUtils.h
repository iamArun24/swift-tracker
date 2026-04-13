// include/utils/DateTimeUtils.h
#pragma once
#include <string>

class DateTimeUtils {
public:
    // Get current UTC time as ISO 8601 string
    static std::string now();
    
    // Get current local time as ISO 8601 string
    static std::string now_local();
    
    // Add hours to a datetime string
    static std::string add_hours(const std::string& datetime, int hours);
    
    // Add days to a datetime string
    static std::string add_days(const std::string& datetime, int days);
    
    // Format datetime for display
    static std::string format_display(const std::string& datetime);
    
    // Parse datetime string to timestamp
    static long to_timestamp(const std::string& datetime);
    
    // Generate UUID v4
    static std::string generate_uuid();
};
