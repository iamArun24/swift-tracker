// src/services/NotificationService.cpp
#include "../../include/services/NotificationService.h"
#include "../../include/database/NotificationRepository.h"
#include "../../include/utils/DateTimeUtils.h"

bool NotificationService::create_notification(int user_id, const std::string& title, const std::string& message, 
                                              const std::string& type, int related_id, const std::string& related_type) {
    Notification notification;
    notification.user_id = user_id;
    notification.title = title;
    notification.message = message;
    notification.type = type;
    notification.is_read = false;
    notification.related_id = related_id;
    notification.related_type = related_type;
    notification.created_at = DateTimeUtils::now();
    
    return NotificationRepository::create(notification);
}

std::vector<Notification> NotificationService::get_user_notifications(int user_id, int limit, int offset) {
    return NotificationRepository::get_by_user_id(user_id, limit, offset);
}

int NotificationService::get_unread_count(int user_id) {
    return NotificationRepository::get_unread_count(user_id);
}

bool NotificationService::mark_as_read(int notification_id, int user_id) {
    return NotificationRepository::mark_as_read(notification_id);
}

bool NotificationService::mark_all_as_read(int user_id) {
    return NotificationRepository::mark_all_as_read(user_id);
}

void NotificationService::broadcast(const nlohmann::json& notification) {
    // WebSocket broadcast will be implemented in main.cpp
    // This is a placeholder for future implementation
}
