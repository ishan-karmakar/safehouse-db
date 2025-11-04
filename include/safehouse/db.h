#include <span>
#include <optional>
#include "safehouse/table.h"

namespace safehouse {
namespace db {

class DB {
public:
    DB();
    void create_table(std::string name, std::unordered_map<std::string, DataType>);
    std::optional<std::unordered_map<std::string, DataType>> get_schema(std::string name);
    Table& get_table(std::string);

private:
    size_t get_num_tables();
    void set_num_tables(size_t num_tables);

    std::fstream file;

    /// Tables that were loaded into memory
    std::unordered_map<std::string, Table> tables;
};

}
}