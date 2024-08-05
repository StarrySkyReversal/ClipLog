#include "pch.h"
#include "database.h"

void InitializeDatabase(const std::string& dbname)
{
    sqlite3* db;
    char* errMsg = 0;

    int rc = sqlite3_open(dbname.c_str(), &db);
    if (rc)
    {
        std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
        return;
    }

    const char* sql = "CREATE TABLE IF NOT EXISTS ClipboardData (id INTEGER PRIMARY KEY AUTOINCREMENT, data TEXT, created_at TEXT);";
    rc = sqlite3_exec(db, sql, 0, 0, &errMsg);
    if (rc != SQLITE_OK)
    {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
    }

    sqlite3_close(db);
}

int SaveDataToDatabase(const std::string& dbname, const std::string& data, const std::string& created_at)
{
    sqlite3* db;
    int rc = sqlite3_open(dbname.c_str(), &db);
    if (rc)
    {
        std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
        return 0;
    }

    std::string insertSQL = "INSERT INTO ClipboardData (data, created_at) VALUES (?, ?);";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, insertSQL.c_str(), -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, data.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, created_at.c_str(), -1, SQLITE_STATIC);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    int lastInsertRowID = static_cast<int>(sqlite3_last_insert_rowid(db));
    sqlite3_close(db);

    return lastInsertRowID;
}

std::pair<std::string, std::string> LoadDataFromDatabase(const std::string& dbname)
{
    sqlite3* db;
    int rc = sqlite3_open(dbname.c_str(), &db);
    if (rc)
    {
        std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
        return std::make_pair("", "");
    }

    std::string selectSQL = "SELECT data, created_at FROM ClipboardData ORDER BY id DESC LIMIT 1;";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, selectSQL.c_str(), -1, &stmt, NULL);

    std::string data;
    std::string created_at;
    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        data = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        created_at = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return std::make_pair(data, created_at);
}

void DeleteDataFromDatabase(const std::string& dbname, int id)
{
    sqlite3* db;
    int rc = sqlite3_open(dbname.c_str(), &db);
    if (rc)
    {
        std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
        return;
    }

    std::string deleteSQL = "DELETE FROM ClipboardData WHERE id = ?;";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, deleteSQL.c_str(), -1, &stmt, NULL);
    sqlite3_bind_int(stmt, 1, id);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    sqlite3_close(db);
}

int GetRecordCount(const std::string& dbname)
{
    sqlite3* db;
    int rc = sqlite3_open(dbname.c_str(), &db);
    if (rc)
    {
        std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
        return 0;
    }

    std::string countSQL = "SELECT COUNT(*) FROM ClipboardData;";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, countSQL.c_str(), -1, &stmt, NULL);

    int count = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        count = sqlite3_column_int(stmt, 0);
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return count;
}

std::vector<ClipboardRecord> LoadAllDataFromDatabase(const std::string& dbname)
{
    sqlite3* db;
    int rc = sqlite3_open(dbname.c_str(), &db);
    if (rc)
    {
        std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
        return {};
    }

    std::string selectSQL = "SELECT id, data, created_at FROM ClipboardData ORDER BY id DESC LIMIT 100;";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, selectSQL.c_str(), -1, &stmt, NULL);

    std::vector<ClipboardRecord> dataVector;
    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        ClipboardRecord record;
        record.id = sqlite3_column_int(stmt, 0);
        record.data = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        record.created_at = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        dataVector.push_back(record);
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return dataVector;
}

// 查询数据库中的指定ID记录
ClipboardRecord GetRecordById(const std::string& dbname, int id)
{
    sqlite3* db;
    int rc = sqlite3_open(dbname.c_str(), &db);
    if (rc)
    {
        std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
        return {};
    }

    std::string selectSQL = "SELECT id, data, created_at FROM ClipboardData WHERE id = ?;";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, selectSQL.c_str(), -1, &stmt, NULL);
    sqlite3_bind_int(stmt, 1, id);

    ClipboardRecord record;
    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        record.id = sqlite3_column_int(stmt, 0);
        record.data = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        record.created_at = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return record;
}
