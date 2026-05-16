// Copyright (C) 2026 Magomed Gadzhiumarov
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef DATABASE_MANAGER_H
#define DATABASE_MANAGER_H

#include <sqlite3.h>
#include <memory>
#include <optional>
#include <string>

class DatabaseManager 
{
public:
    static DatabaseManager& instance();

    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

    bool insertNewUser(const std::string& login, const std::string& passHash);
    std::optional<std::string> findHashPass(const std::string& login);

private:
    // Конструктор в привате
    DatabaseManager(const std::string& dbName);
    ~DatabaseManager() = default;

    struct SQLiteDeleter {
        void operator()(sqlite3* p) const { sqlite3_close(p); }
        void operator()(sqlite3_stmt* p) const { sqlite3_finalize(p); }
    };

    using UniqueDb = std::unique_ptr<sqlite3, SQLiteDeleter>;
    using UniqueStmt = std::unique_ptr<sqlite3_stmt, SQLiteDeleter>;

    std::optional<UniqueStmt> prepare(const std::string& sql);
    void executeRaw(const std::string& sql);

    UniqueDb db;
};

#endif