// include/database/NotificationRepository.h
#pragma once
#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include "../models/Notification.h"

class NotificationRepository {
public:
    // Create notification
    static bool create(const Notification& notification);
    
    // Get notifications for user
    static std::vector<Notification> get_by_user_id(int user_id, int limit = 50, int offset = 0);
    
    // Get unread count for user
    static int get_unread_count(int user_id);
    
    // Mark as read
    static bool mark_as_read(int notification_id);
    
    // Mark all as read for user
    static bool mark_all_as_read(int user_id);
    
    // Delete notification
    static bool remove(int notification_id);
};
