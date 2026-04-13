// src/database/NotificationRepository.cpp
#include "../../include/database/NotificationRepository.h"
#include "../../include/database/DatabaseManager.h"
#include <sstream>

bool NotificationRepository::create(const Notification& notification) {
    auto& db = DatabaseManager::get_instance();
    
    std::ostringstream sql;
    sql << "INSERT INTO notifications (user_id, title, message, type, is_read, related_id, related_type, created_at) VALUES ("
        << notification.user_id << ", '"
        << notification.title << "', '"
        << notification.message << "', '"
        << notification.type << "', "
        << (notification.is_read ? 1 : 0) << ", "
        << notification.related_id << ", '"
        << notification.related_type << "', '"
        << notification.created_at << "')";
    
    return db.execute(sql.str());
}

std::vector<Notification> NotificationRepository::get_by_user_id(int user_id, int limit, int offset) {
    auto& db = DatabaseManager::get_instance();
    
    std::ostringstream sql;
    sql << "SELECT * FROM notifications WHERE user_id = " << user_id
        << " ORDER BY created_at DESC LIMIT " << limit << " OFFSET " << offset;
    
    auto results = db.query(sql.str());
    std::vector<Notification> notifications;
    
    for (const auto& row : results) {
        Notification notification;
        notification.id = std::stoi(row.at("id"));
        notification.user_id = std::stoi(row.at("user_id"));
        notification.title = row.at("title");
        notification.message = row.at("message");
        notification.type = row.at("type");
        notification.is_read = row.at("is_read") == "1";
        notification.related_id = row.at("related_id").empty() ? 0 : std::stoi(row.at("related_id"));
        notification.related_type = row.at("related_type");
        notification.created_at = row.at("created_at");
        notifications.push_back(notification);
    }
    
    return notifications;
}

int NotificationRepository::get_unread_count(int user_id) {
    auto& db = DatabaseManager::get_instance();
    
    std::ostringstream sql;
    sql << "SELECT COUNT(*) as count FROM notifications WHERE user_id = " << user_id
        << " AND is_read = 0";
    
    auto result = db.query_one(sql.str());
    return std::stoi(result["count"]);
}

bool NotificationRepository::mark_as_read(int notification_id) {
    auto& db = DatabaseManager::get_instance();
    
    std::ostringstream sql;
    sql << "UPDATE notifications SET is_read = 1 WHERE id = " << notification_id;
    
    return db.execute(sql.str());
}

bool NotificationRepository::mark_all_as_read(int user_id) {
    auto& db = DatabaseManager::get_instance();
    
    std::ostringstream sql;
    sql << "UPDATE notifications SET is_read = 1 WHERE user_id = " << user_id;
    
    return db.execute(sql.str());
}

bool NotificationRepository::remove(int notification_id) {
    auto& db = DatabaseManager::get_instance();
    
    std::ostringstream sql;
    sql << "DELETE FROM notifications WHERE id = " << notification_id;
    
    return db.execute(sql.str());
}
