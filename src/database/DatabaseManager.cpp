// src/database/DatabaseManager.cpp
#include "../../include/database/DatabaseManager.h"
#include <iostream>
#include <fstream>

std::unique_ptr<DatabaseManager> DatabaseManager::instance_ = nullptr;

DatabaseManager::DatabaseManager(const std::string& path) : db_(nullptr), db_path_(path) {
    if (sqlite3_open(path.c_str(), &db_) != SQLITE_OK) {
        std::cerr << "[DatabaseManager] Failed to open database: " << sqlite3_errmsg(db_) << std::endl;
        db_ = nullptr;
    } else {
        std::cout << "[DatabaseManager] Database opened: " << path << std::endl;
    }
}

DatabaseManager::~DatabaseManager() {
    if (db_) {
        sqlite3_close(db_);
        std::cout << "[DatabaseManager] Database closed" << std::endl;
    }
}

DatabaseManager& DatabaseManager::get_instance(const std::string& path) {
    if (!instance_) {
        instance_ = std::unique_ptr<DatabaseManager>(new DatabaseManager(path));
    }
    return *instance_;
}

bool DatabaseManager::initialize() {
    if (!db_) {
        std::cerr << "[DatabaseManager] Database not initialized" << std::endl;
        return false;
    }
    
    // Read and execute schema file
    std::string schema_path = "./sql/schema.sql";
    std::ifstream schema_file(schema_path);
    
    if (!schema_file.is_open()) {
        std::cerr << "[DatabaseManager] Schema file not found: " << schema_path << std::endl;
        return false;
    }
    
    std::string sql((std::istreambuf_iterator<char>(schema_file)),
                    std::istreambuf_iterator<char>());
    schema_file.close();
    
    // Execute schema
    if (!execute(sql)) {
        std::cerr << "[DatabaseManager] Failed to execute schema" << std::endl;
        return false;
    }
    
    std::cout << "[DatabaseManager] Database initialized successfully" << std::endl;
    return true;
}

bool DatabaseManager::execute(const std::string& sql) {
    if (!db_) return false;
    
    char* err_msg = nullptr;
    int rc = sqlite3_exec(db_, sql.c_str(), nullptr, nullptr, &err_msg);
    
    if (rc != SQLITE_OK) {
        std::cerr << "[DatabaseManager] SQL error: " << err_msg << std::endl;
        sqlite3_free(err_msg);
        return false;
    }
    
    return true;
}

std::vector<std::map<std::string, std::string>> DatabaseManager::query(const std::string& sql) {
    std::vector<std::map<std::string, std::string>> results;
    
    if (!db_) return results;
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "[DatabaseManager] Prepare error: " << sqlite3_errmsg(db_) << std::endl;
        return results;
    }
    
    int column_count = sqlite3_column_count(stmt);
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        std::map<std::string, std::string> row;
        
        for (int i = 0; i < column_count; i++) {
            const char* col_name = sqlite3_column_name(stmt, i);
            const char* col_value = reinterpret_cast<const char*>(sqlite3_column_text(stmt, i));
            
            if (col_value) {
                row[col_name] = col_value;
            } else {
                row[col_name] = "";
            }
        }
        
        results.push_back(row);
    }
    
    sqlite3_finalize(stmt);
    return results;
}

std::map<std::string, std::string> DatabaseManager::query_one(const std::string& sql) {
    auto results = query(sql);
    if (results.empty()) {
        return {};
    }
    return results[0];
}

int64_t DatabaseManager::last_insert_id() {
    if (!db_) return 0;
    return sqlite3_last_insert_rowid(db_);
}

bool DatabaseManager::begin_transaction() {
    return execute("BEGIN TRANSACTION;");
}

bool DatabaseManager::commit() {
    return execute("COMMIT;");
}

bool DatabaseManager::rollback() {
    return execute("ROLLBACK;");
}

bool DatabaseManager::is_open() const {
    return db_ != nullptr;
}

void DatabaseManager::close() {
    if (db_) {
        sqlite3_close(db_);
        db_ = nullptr;
    }
}
