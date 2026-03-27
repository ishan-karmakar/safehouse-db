#include <filesystem>
#include <spdlog/spdlog.h>
#include <magic_enum/magic_enum.hpp>
#include "safehouse/table.h"
#include "safehouse/db.h"

using namespace safehouse::db;

Table::Table(std::string name, DB& db) : columns{*db.get_schema(name)} {
    std::string filename = name + ".db";
    if (!std::filesystem::exists(filename))
        std::ofstream{filename, std::ios::out | std::ios::binary}.close();
    file.open(filename, std::ios::in | std::ios::out | std::ios::binary);
}

void Table::insert(nlohmann::json data) {
    if (data.size() != columns.size()) {
        spdlog::error("Number of columns are not the same in schema and data");
        return;
    }
    std::vector<nlohmann::json> insert_data;
    for (const auto& [key, value] : data.items()) {
        if (!columns.contains(key)) {
            spdlog::error("Column '{}' does not exist in schema", key);
            return;
        }
        auto col_type = columns.at(key);
        if (
            (col_type == DataType::Decimal && !value.is_number_float()) ||
            (col_type == DataType::Number && !value.is_number_integer())
        ) {
            spdlog::error("Column '{}' does not match the schema type ({})", key, magic_enum::enum_name(col_type));
            return;
        }
        insert_data.emplace_back(value);
    }

    for (const auto& value : insert_data) {
    }
}