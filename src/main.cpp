#include <iostream>
#include "safehouse/db.h"

int main() {
    safehouse::db::DB db;
    db.create_table("users", {
        {"id", safehouse::db::Number},
        {"name", safehouse::db::String},
        {"email", safehouse::db::String}
    });
    safehouse::db::Table& table = db.get_table("users");
    return 0;
}