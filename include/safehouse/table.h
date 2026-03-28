#pragma once
#include <stddef.h>
#include "safehouse/pager.h"

#define TABLE_MAX_PAGES 100

typedef struct {
    Pager *pager;
    size_t root_page_num;
} Table;

typedef struct {
    Table *table;
    size_t page_num;
    size_t cell_num;
    bool eof;
} Cursor;

Table *db_open(const char *filename);
void db_close(Table*);
Cursor *table_start(Table *table);
Cursor *table_end(Table *table);
void *cursor_value(Cursor *cursor);
void cursor_advance(Cursor *cursor);

void leaf_node_insert(Cursor *cursor, size_t key, Row *value);