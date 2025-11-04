#include <filesystem>
#include "safehouse/table.h"
#include "safehouse/db.h"

using namespace safehouse::db;

Table::Table(std::string name, DB& db) : columns{*db.get_schema(name + ".db")} {
    std::string filename = name + ".db";
    if (!std::filesystem::exists(filename))
        std::ofstream{filename, std::ios::out | std::ios::binary}.close();
    file.open(filename, std::ios::in | std::ios::out | std::ios::binary);
}