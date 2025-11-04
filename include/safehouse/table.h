#pragma once
#include <string>
#include <fstream>
#include <unordered_map>

namespace safehouse {
namespace db {

class DB;

enum DataType {
    Number,
    String,
    Decimal
};

class Table {
public:
    Table(std::string name, DB& db);

private:
    std::unordered_map<std::string, DataType> columns;
    std::fstream file;
};

}
}