#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>
#include <fstream>

namespace safehouse {
namespace pager {

class Pager {
public:
    Pager(std::string filename);
    void *get(size_t page);

private:
    size_t get_file_size();

    std::fstream file;
    size_t num_pages;
    std::unordered_map<size_t, std::unique_ptr<char>> pages;
};

}
}