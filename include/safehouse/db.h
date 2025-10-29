#include <span>
#include <optional>
#include "safehouse/pager.h"

namespace safehouse {
namespace db {

enum DataType {
    Number,
    String,
    Decimal
};

class DB {
public:
    DB();
    void create_table(std::string name, std::unordered_map<std::string, DataType>);
    std::optional<std::unordered_map<std::string, DataType>> find_table(std::string name);

private:
    size_t get_num_tables();
    void set_num_tables(size_t num_tables);

    std::fstream table_file;
    std::fstream data_file;
};

}
}