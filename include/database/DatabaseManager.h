// include/database/DatabaseManager.h
#pragma once
#include <string>
#include <vector>
#include <map>
#include <sqlite3.h>
#include <memory>

class DatabaseManager {
private:
    sqlite3* db_;
    std::string db_path_;
    static std::unique_ptr<DatabaseManager> instance_;
    
    DatabaseManager(const std::string& path);
    
public:
    ~DatabaseManager();
    
    // Singleton pattern
    static DatabaseManager& get_instance(const std::string& path = "./swifttrack.db");
    
    // Initialize database (create tables)
    bool initialize();
    
    // Execute SQL (INSERT, UPDATE, DELETE)
    bool execute(const std::string& sql);
    
    // Query SQL (SELECT) - returns vector of rows
    // Each row is a map of column_name -> value (as string)
    std::vector<std::map<std::string, std::string>> query(const std::string& sql);
    
    // Get single row by ID
    std::map<std::string, std::string> query_one(const std::string& sql);
    
    // Get last insert row ID
    int64_t last_insert_id();
    
    // Begin transaction
    bool begin_transaction();
    
    // Commit transaction
    bool commit();
    
    // Rollback transaction
    bool rollback();
    
    // Check if database is open
    bool is_open() const;
    
    // Close database
    void close();
};
