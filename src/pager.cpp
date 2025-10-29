#include <filesystem>
#include <spdlog/spdlog.h>
#include "safehouse/pager.h"

using namespace safehouse::pager;
constexpr size_t PAGE_SIZE = 0x1000;

Pager::Pager(std::string filename) : file{filename} {
    size_t file_length = get_file_size();
    if (file_length % PAGE_SIZE) {
        spdlog::critical("File is not a whole number of pages. Corrupt file");
        std::exit(EXIT_FAILURE);
    }
    num_pages = file_length / PAGE_SIZE;
}

void *Pager::get(size_t page_num) {
    if (!pages.contains(page_num)) {
        std::unique_ptr<char> page{new char[PAGE_SIZE]};
        if (page_num < num_pages) {
            file.seekg(page_num * PAGE_SIZE, std::ios::beg);
            file.read(page.get(), PAGE_SIZE);
            if (file.fail())
                spdlog::error("Error reading from file at page {}", page_num);
        }
        pages[page_num] = std::move(page);
        num_pages = std::max(num_pages, page_num + 1);
    }
    return pages.at(page_num).get();
}

size_t Pager::get_file_size() {
    file.ignore(std::numeric_limits<std::streamsize>::max());
    std::streamsize length = file.gcount();
    file.clear();
    file.seekg(0, std::ios::beg);
    return length;
}
