#include <iostream>
#include "safehouse/db.h"

int main() {
    safehouse::db::DB db;
    db.create_table("users", {
        {"id", safehouse::db::Number},
        {"name", safehouse::db::String},
        {"email", safehouse::db::String}
    });
    // db.get_schema("users");
    return 0;
}