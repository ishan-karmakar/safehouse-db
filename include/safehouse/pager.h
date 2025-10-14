#pragma once
#include <cstdint>
#include <vector>
#include <string>

namespace safehouse {
namespace db {

class Pager {
    Pager(std::string filename);
    ~Pager();

private:
    std::vector<void*> pages;
};

}
}