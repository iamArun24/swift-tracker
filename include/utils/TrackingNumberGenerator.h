// include/utils/TrackingNumberGenerator.h
#pragma once
#include <string>
#include <atomic>

class TrackingNumberGenerator {
private:
    static std::atomic<int> counter_;
    
public:
    // Generate tracking number format: SWT + YYYYMMDD + 7 random alphanum + 2 digit counter
    // Example: SWT20240115A3K7P2M12
    static std::string generate();
    
    // Validate tracking number format
    static bool validate(const std::string& tracking_no);
};
