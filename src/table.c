#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stumpless.h>
#include "safehouse/table.h"
#include "safehouse/row.h"
#include "safehouse/node.h"

Table *db_open(const char *filename) {
    Pager *pager = pager_open(filename);
    size_t num_rows = pager->file_length / ROW_SIZE;

    Table *table = malloc(sizeof(Table));
    table->pager = pager;
    table->root_page_num = 0;
    if (pager->num_pages == 0) {
        void *root_node = pager_get(pager, 0);
        initialize_leaf_node(root_node);
    }
    return table;
}

void db_close(Table *table) {
    Pager *pager = table->pager;
    for (size_t i = 0; i < pager->num_pages; i++) {
        if (pager->pages[i] = NULL)
            continue;
        pager_flush(pager, i);
        free(pager->pages[i]);
        pager->pages[i] = NULL;
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

Cursor *table_start(Table *table) {
    Cursor *cursor = malloc(sizeof(Cursor));
    cursor->table = table;
    cursor->page_num = table->root_page_num;
    cursor->cell_num = 0;

    void *root_node = pager_get(table->pager, table->root_page_num);
    size_t num_cells = *leaf_node_num_cells(root_node);
    cursor->eof = num_cells == 0;
    return cursor;
}

Cursor *table_end(Table *table) {
    Cursor *cursor = malloc(sizeof(Cursor));
    cursor->table = table;
    void *root_node = pager_get(table->pager, table->root_page_num);
    size_t num_cells = *leaf_node_num_cells(root_node);
    cursor->cell_num = num_cells;
    cursor->eof = true;
    return cursor;
}

void *cursor_value(Cursor *cursor) {
    void *page = pager_get(cursor->table->pager, cursor->page_num);
    return leaf_node_value(cursor->page_num, cursor->cell_num);
}

void cursor_advance(Cursor *cursor) {
    void *node = pager_get(cursor->table->pager, cursor->page_num);
    cursor->eof = ++cursor->cell_num >= *leaf_node_num_cells(node);
}

void leaf_node_insert(Cursor *cursor, size_t key, Row *row) {
    void *node = pager_get(cursor->table->pager, cursor->page_num);
    size_t num_cells = *leaf_node_num_cells(node);
    if (num_cells >= LEAF_NODE_MAX_CELLS) {
        stump_c("Need to implement splitting a leaf node.");
        exit(EXIT_FAILURE);
    }

    if (cursor->cell_num < num_cells) {
        for (size_t i = num_cells; i > cursor->cell_num; i--)
            memcpy(leaf_node_cell(node, i), leaf_node_cell(node, i - 1), LEAF_NODE_CELL_SIZE);
    }

    (*leaf_node_num_cells(node))++;
    *leaf_node_key(node, cursor->cell_num) = key;
    row_serialize(row, leaf_node_value(node, cursor->cell_num));
}