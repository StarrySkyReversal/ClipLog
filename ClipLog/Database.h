#pragma once

#include "sqlite3/sqlite3.h"
#include <iostream>
#include <string>
#include <codecvt>
#include <locale>
#include <ctime>
#include <vector>
#include <utility>

struct ClipboardRecord
{
    int id;
    std::string data;
    std::string created_at;
};

void InitializeDatabase(const std::string& dbname);

int SaveDataToDatabase(const std::string& dbname, const std::string& data, const std::string& created_at);

std::pair<std::string, std::string> LoadDataFromDatabase(const std::string& dbname);

void DeleteDataFromDatabase(const std::string& dbname, int id);

//void UpdateDataInDatabase(const std::string& dbname, int id, const std::string& newData);

int GetRecordCount(const std::string& dbname);

std::vector<ClipboardRecord> LoadAllDataFromDatabase(const std::string& dbname);

ClipboardRecord GetRecordById(const std::string& dbname, int id);