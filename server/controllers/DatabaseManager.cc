#include "DatabaseManager.h"
#include <stdexcept>

DatabaseManager& DatabaseManager::instance() 
{
    static DatabaseManager inst("users.db");
    return inst;
}

DatabaseManager::DatabaseManager(const std::string& dbName) 
{
    sqlite3* raw_ptr = nullptr;
    if (sqlite3_open(dbName.c_str(), &raw_ptr) != SQLITE_OK) {
        if (raw_ptr) sqlite3_close(raw_ptr);
        throw std::runtime_error("Can't open DB: " + dbName);
    }
    db.reset(raw_ptr);

    executeRaw("CREATE TABLE IF NOT EXISTS USERS ("
               "ID INTEGER PRIMARY KEY AUTOINCREMENT,"
               "LOGIN TEXT NOT NULL UNIQUE,"
               "PASSWORD_HASH TEXT NOT NULL);");
}

void DatabaseManager::executeRaw(const std::string& sql) 
{
    char* errMsg = nullptr;
    if (sqlite3_exec(db.get(), sql.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::string err = errMsg ? errMsg : "Unknown SQL error";
        if (errMsg) sqlite3_free(errMsg);
        throw std::runtime_error("SQL Error: " + err);
    }
}

std::optional<DatabaseManager::UniqueStmt> DatabaseManager::prepare(const std::string& sql) 
{
    sqlite3_stmt* raw_stmt = nullptr;
    if (sqlite3_prepare_v2(db.get(), sql.c_str(), -1, &raw_stmt, nullptr) != SQLITE_OK) {
        return std::nullopt;
    }
    return UniqueStmt(raw_stmt);
}

bool DatabaseManager::insertNewUser(const std::string& login, const std::string& passHash) 
{
    auto stmtOpt = prepare("INSERT INTO USERS (LOGIN, PASSWORD_HASH) VALUES (?, ?);");
    if (!stmtOpt) return false;

    auto& stmt = *stmtOpt;
    sqlite3_bind_text(stmt.get(), 1, login.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt.get(), 2, passHash.c_str(), -1, SQLITE_STATIC);
    
    return sqlite3_step(stmt.get()) == SQLITE_DONE;
}

std::optional<std::string> DatabaseManager::findHashPass(const std::string& login) 
{
    auto stmtOpt = prepare("SELECT PASSWORD_HASH FROM USERS WHERE LOGIN = ?;");
    if (!stmtOpt) return std::nullopt;

    auto& stmt = *stmtOpt;
    sqlite3_bind_text(stmt.get(), 1, login.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt.get()) == SQLITE_ROW) {
        const char* text = reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), 0));
        if (text) return std::string(text);
    }
    return std::nullopt;
}