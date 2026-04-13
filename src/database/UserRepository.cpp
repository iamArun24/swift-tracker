// src/database/UserRepository.cpp
#include "../../include/database/UserRepository.h"
#include "../../include/database/DatabaseManager.h"
#include "../../include/utils/PasswordUtils.h"
#include "../../include/utils/DateTimeUtils.h"
#include <sstream>

bool UserRepository::create(const User& user) {
    auto& db = DatabaseManager::get_instance();
    
    std::ostringstream sql;
    sql << "INSERT INTO users (uuid, full_name, email, phone, password_hash, role, "
        << "city, state, pincode, address, is_active, created_at, updated_at) VALUES ('"
        << user.uuid << "', '"
        << user.full_name << "', '"
        << user.email << "', '"
        << user.phone << "', '"
        << user.password_hash << "', '"
        << role_to_string(user.role) << "', '"
        << user.city << "', '"
        << user.state << "', '"
        << user.pincode << "', '"
        << user.address << "', "
        << (user.is_active ? 1 : 0) << ", '"
        << user.created_at << "', '"
        << user.updated_at << "')";
    
    return db.execute(sql.str());
}

std::optional<User> UserRepository::find_by_id(int id) {
    auto& db = DatabaseManager::get_instance();
    
    std::ostringstream sql;
    sql << "SELECT * FROM users WHERE id = " << id;
    
    auto result = db.query_one(sql.str());
    if (result.empty()) {
        return std::nullopt;
    }
    
    User user;
    user.id = std::stoi(result["id"]);
    user.uuid = result["uuid"];
    user.full_name = result["full_name"];
    user.email = result["email"];
    user.phone = result["phone"];
    user.password_hash = result["password_hash"];
    user.role = string_to_role(result["role"]);
    user.is_active = result["is_active"] == "1";
    user.city = result["city"];
    user.state = result["state"];
    user.pincode = result["pincode"];
    user.address = result["address"];
    user.created_at = result["created_at"];
    user.updated_at = result["updated_at"];
    user.last_login = result["last_login"];
    
    return user;
}

std::optional<User> UserRepository::find_by_email(const std::string& email) {
    auto& db = DatabaseManager::get_instance();
    
    std::ostringstream sql;
    sql << "SELECT * FROM users WHERE email = '" << email << "'";
    
    auto result = db.query_one(sql.str());
    if (result.empty()) {
        return std::nullopt;
    }
    
    User user;
    user.id = std::stoi(result["id"]);
    user.uuid = result["uuid"];
    user.full_name = result["full_name"];
    user.email = result["email"];
    user.phone = result["phone"];
    user.password_hash = result["password_hash"];
    user.role = string_to_role(result["role"]);
    user.is_active = result["is_active"] == "1";
    user.city = result["city"];
    user.state = result["state"];
    user.pincode = result["pincode"];
    user.address = result["address"];
    user.created_at = result["created_at"];
    user.updated_at = result["updated_at"];
    user.last_login = result["last_login"];
    
    return user;
}

std::optional<User> UserRepository::find_by_uuid(const std::string& uuid) {
    auto& db = DatabaseManager::get_instance();
    
    std::ostringstream sql;
    sql << "SELECT * FROM users WHERE uuid = '" << uuid << "'";
    
    auto result = db.query_one(sql.str());
    if (result.empty()) {
        return std::nullopt;
    }
    
    User user;
    user.id = std::stoi(result["id"]);
    user.uuid = result["uuid"];
    user.full_name = result["full_name"];
    user.email = result["email"];
    user.phone = result["phone"];
    user.password_hash = result["password_hash"];
    user.role = string_to_role(result["role"]);
    user.is_active = result["is_active"] == "1";
    user.city = result["city"];
    user.state = result["state"];
    user.pincode = result["pincode"];
    user.address = result["address"];
    user.created_at = result["created_at"];
    user.updated_at = result["updated_at"];
    user.last_login = result["last_login"];
    
    return user;
}

bool UserRepository::update(const User& user) {
    auto& db = DatabaseManager::get_instance();
    
    std::ostringstream sql;
    sql << "UPDATE users SET full_name = '" << user.full_name
        << "', phone = '" << user.phone
        << "', city = '" << user.city
        << "', state = '" << user.state
        << "', pincode = '" << user.pincode
        << "', address = '" << user.address
        << "', updated_at = '" << DateTimeUtils::now()
        << "' WHERE id = " << user.id;
    
    return db.execute(sql.str());
}

bool UserRepository::update_last_login(int user_id) {
    auto& db = DatabaseManager::get_instance();
    
    std::ostringstream sql;
    sql << "UPDATE users SET last_login = '" << DateTimeUtils::now()
        << "' WHERE id = " << user_id;
    
    return db.execute(sql.str());
}

bool UserRepository::update_password(int user_id, const std::string& new_hash) {
    auto& db = DatabaseManager::get_instance();
    
    std::ostringstream sql;
    sql << "UPDATE users SET password_hash = '" << new_hash
        << "', updated_at = '" << DateTimeUtils::now()
        << "' WHERE id = " << user_id;
    
    return db.execute(sql.str());
}

bool UserRepository::deactivate(int user_id) {
    auto& db = DatabaseManager::get_instance();
    
    std::ostringstream sql;
    sql << "UPDATE users SET is_active = 0, updated_at = '" << DateTimeUtils::now()
        << "' WHERE id = " << user_id;
    
    return db.execute(sql.str());
}

std::vector<User> UserRepository::get_all(const std::string& role, bool active_only) {
    auto& db = DatabaseManager::get_instance();
    
    std::ostringstream sql;
    sql << "SELECT * FROM users WHERE 1=1";
    
    if (!role.empty()) {
        sql << " AND role = '" << role << "'";
    }
    
    if (active_only) {
        sql << " AND is_active = 1";
    }
    
    sql << " ORDER BY created_at DESC";
    
    auto results = db.query(sql.str());
    std::vector<User> users;
    
    for (const auto& row : results) {
        User user;
        user.id = std::stoi(row.at("id"));
        user.uuid = row.at("uuid");
        user.full_name = row.at("full_name");
        user.email = row.at("email");
        user.phone = row.at("phone");
        user.role = string_to_role(row.at("role"));
        user.is_active = row.at("is_active") == "1";
        user.city = row.at("city");
        user.state = row.at("state");
        user.pincode = row.at("pincode");
        user.address = row.at("address");
        user.created_at = row.at("created_at");
        user.updated_at = row.at("updated_at");
        user.last_login = row.at("last_login");
        users.push_back(user);
    }
    
    return users;
}

bool UserRepository::email_exists(const std::string& email) {
    auto& db = DatabaseManager::get_instance();
    
    std::ostringstream sql;
    sql << "SELECT COUNT(*) as count FROM users WHERE email = '" << email << "'";
    
    auto result = db.query_one(sql.str());
    return std::stoi(result["count"]) > 0;
}
