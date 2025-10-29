#include "safehouse/db.h"
#include <iostream>
#include <spdlog/spdlog.h>
using namespace safehouse::db;

typedef char num_fields_t;

constexpr size_t NUM_TABLES_OFFSET = 0;

DB::DB() : table_file{"tables.db", std::ios::in | std::ios::out | std::ios::binary}, data_file{"data.db", std::ios::in | std::ios::out | std::ios::binary} {
    if (!table_file.is_open()) {
        {
            std::ofstream file{"tables.db", std::ios::binary};
        }
        table_file.open("tables.db", std::ios::in | std::ios::out | std::ios::binary);
        set_num_tables(0);
    }
    if (!data_file.is_open()) {
        {
            std::ofstream file{"data.db", std::ios::binary};
        }
        data_file.open("data.db", std::ios::in | std::ios::out | std::ios::binary);
    }
}

void DB::create_table(std::string name, std::unordered_map<std::string, DataType> fields) {
    table_file.seekp(0, std::ios::end);
    num_fields_t num_fields = fields.size();
    table_file.write(reinterpret_cast<char*>(&num_fields), sizeof(num_fields));
    table_file.write(name.c_str(), name.size() + 1);
    for (const auto& field : fields) {
        table_file.write(field.first.c_str(), field.first.size() + 1);
        table_file.write(reinterpret_cast<const char*>(&field.second), sizeof(field.second));
    }
    set_num_tables(get_num_tables() + 1);
}

std::optional<std::unordered_map<std::string, DataType>> DB::find_table(std::string name) {
    table_file.seekg(NUM_TABLES_OFFSET + sizeof(size_t));
    while (true) {
        num_fields_t num_fields;
        table_file.read(reinterpret_cast<char*>(&num_fields), sizeof(num_fields));
        if (table_file.eof()) {
            spdlog::warn("Could not find table {}", name);
            return std::nullopt;
        }
        std::string table_name;
        std::getline(table_file, table_name, '\0');
        if (table_name == name) {
            std::unordered_map<std::string, DataType> fields;
            fields.reserve(num_fields);
            for (int i = 0; i < num_fields; i++) {
                std::string field_name;
                std::getline(table_file, field_name, '\0');
                DataType field_type;
                table_file.read(reinterpret_cast<char*>(&field_type), sizeof(field_type));
                fields.insert({ field_name, field_type });
            }
            return fields;
        } else {
            for (int i = 0; i < num_fields; i++) {
                table_file.ignore(std::numeric_limits<size_t>::max(), '\0');
                table_file.seekg(sizeof(DataType), std::ios::cur);
            }
        }
    }
}

size_t DB::get_num_tables() {
    size_t num_tables;
    table_file.seekg(NUM_TABLES_OFFSET);
    table_file.read(reinterpret_cast<char*>(&num_tables), sizeof(num_tables));
    return num_tables;
}

void DB::set_num_tables(size_t num_tables) {
    table_file.seekp(NUM_TABLES_OFFSET);
    table_file.write(reinterpret_cast<char*>(&num_tables), sizeof(num_tables));
}
