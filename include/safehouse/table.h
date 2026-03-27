#pragma once
#include <string>
#include <fstream>
#include <unordered_map>
#include <nlohmann/json.hpp>

namespace safehouse {
namespace db {

class DB;

enum DataType {
    Number,
    // String,
    Decimal
};

class Table {
public:
    Table(std::string name, DB& db);
    void insert(nlohmann::json data);

private:
    std::unordered_map<std::string, DataType> columns;
    std::fstream file;
};

}
}