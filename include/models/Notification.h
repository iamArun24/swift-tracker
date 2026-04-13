// include/models/Notification.h
#pragma once
#include <string>
#include <nlohmann/json.hpp>

struct Notification {
    int id = 0;
    int user_id = 0;
    std::string title;
    std::string message;
    std::string type;
    bool is_read = false;
    int related_id = 0;
    std::string related_type;
    std::string created_at;
    
    nlohmann::json to_json() const {
        return {
            {"id", id},
            {"user_id", user_id},
            {"title", title},
            {"message", message},
            {"type", type},
            {"is_read", is_read},
            {"related_id", related_id},
            {"related_type", related_type},
            {"created_at", created_at}
        };
    }
};
