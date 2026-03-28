#include <stdlib.h>
#include <unistd.h>
#include <stumpless.h>
#include "safehouse/table.h"
#include "safehouse/row.h"

Table *db_open(const char *filename) {
    Pager *pager = pager_open(filename);
    size_t num_rows = pager->file_length / ROW_SIZE;

    Table *table = malloc(sizeof(Table));
    table->pager = pager;
    table->num_rows = num_rows;
    return table;
}

void db_close(Table *table) {
    Pager *pager = table->pager;
    size_t num_full_pages = table->num_rows / ROWS_PER_PAGE;
    for (size_t i = 0; i < num_full_pages; i++) {
        if (pager->pages[i] = NULL)
            continue;
        pager_flush(pager, i, PAGE_SIZE);
        free(pager->pages[i]);
        pager->pages[i] = NULL;
    }

    size_t num_additional_rows = table->num_rows % ROWS_PER_PAGE;
    if (num_additional_rows > 0) {
        size_t page_num = num_full_pages;
        if (pager->pages[page_num] != NULL) {
            pager_flush(pager, page_num, num_additional_rows * ROW_SIZE);
            free(pager->pages[page_num]);
            pager->pages[page_num] = NULL;
        }
    }

    int result = close(pager->fd);
    if (result == -1) {
        stump_c("Error closing DB file");
        exit(EXIT_FAILURE);
    }

    for (size_t i = 0; i < TABLE_MAX_PAGES; i++) {
        void *page = pager->pages[i];
        if (page) {
            free(page);
            pager->pages[i] = NULL;
        }
    }
    free(pager);
    free(table);
}

void *table_row_slot(Table *table, size_t row_num) {
    size_t page_num = row_num / ROWS_PER_PAGE;
    void *page = pager_get(table->pager, page_num);
    size_t row_offset = row_num % ROWS_PER_PAGE;
    size_t byte_offset = row_offset * ROW_SIZE;
    return page + byte_offset;
}