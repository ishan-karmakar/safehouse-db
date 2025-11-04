#include "safehouse/db.h"
#include <iostream>
#include <filesystem>
#include <spdlog/spdlog.h>
using namespace safehouse::db;

typedef char num_fields_t;

const std::string TABLES_NAME = "tables.db";

DB::DB() {
    file.open(TABLES_NAME, std::ios::in | std::ios::out | std::ios::binary);
    if (!std::filesystem::exists(TABLES_NAME)) {
        std::ofstream{TABLES_NAME, std::ios::out | std::ios::binary}.close();
        file.open(TABLES_NAME, std::ios::in | std::ios::out | std::ios::binary);
    }
}

void DB::create_table(std::string name, std::unordered_map<std::string, DataType> fields) {
    if (get_schema(name)) {
        spdlog::warn("Table {} already exists", name);
        return;
    }
    file.seekp(0, std::ios::end);
    num_fields_t num_fields = fields.size();
    file.write(reinterpret_cast<char*>(&num_fields), sizeof(num_fields));
    file.write(name.c_str(), name.size() + 1);
    for (const auto& field : fields) {
        file.write(field.first.c_str(), field.first.size() + 1);
        file.write(reinterpret_cast<const char*>(&field.second), sizeof(field.second));
    }
    // set_num_tables(get_num_tables() + 1);
}

std::optional<std::unordered_map<std::string, DataType>> DB::get_schema(std::string name) {
    file.seekg(0);
    while (true) {
        num_fields_t num_fields;
        file.read(reinterpret_cast<char*>(&num_fields), sizeof(num_fields));
        if (file.eof())
            return std::nullopt;
        std::string table_name;
        std::getline(file, table_name, '\0');
        if (table_name == name) {
            std::unordered_map<std::string, DataType> fields;
            fields.reserve(num_fields);
            for (int i = 0; i < num_fields; i++) {
                std::string field_name;
                std::getline(file, field_name, '\0');
                DataType field_type;
                file.read(reinterpret_cast<char*>(&field_type), sizeof(field_type));
                fields.insert({ field_name, field_type });
            }
            return fields;
        } else {
            for (int i = 0; i < num_fields; i++) {
                file.ignore(std::numeric_limits<size_t>::max(), '\0');
                file.seekg(sizeof(DataType), std::ios::cur);
            }
        }
    }
}

Table& DB::get_table(std::string name) {
    if (!tables.contains(name))
        tables.emplace(name, Table{name, *this});
    return tables.at(name);
}

// size_t DB::get_num_tables() {
//     size_t num_tables;
//     file.seekg(NUM_TABLES_OFFSET);
//     file.read(reinterpret_cast<char*>(&num_tables), sizeof(num_tables));
//     return num_tables;
// }

// void DB::set_num_tables(size_t num_tables) {
//     file.seekp(NUM_TABLES_OFFSET);
//     file.write(reinterpret_cast<char*>(&num_tables), sizeof(num_tables));
// }
