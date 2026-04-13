// include/services/NotificationService.h
#pragma once
#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include "../models/Notification.h"

class NotificationService {
public:
    // Create notification
    static bool create_notification(int user_id, const std::string& title, const std::string& message, 
                                   const std::string& type = "INFO", int related_id = 0, 
                                   const std::string& related_type = "");
    
    // Get notifications for user
    static std::vector<Notification> get_user_notifications(int user_id, int limit = 50, int offset = 0);
    
    // Get unread count
    static int get_unread_count(int user_id);
    
    // Mark as read
    static bool mark_as_read(int notification_id, int user_id);
    
    // Mark all as read
    static bool mark_all_as_read(int user_id);
    
    // Broadcast notification via WebSocket (for future implementation)
    static void broadcast(const nlohmann::json& notification);
};
