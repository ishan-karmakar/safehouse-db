#pragma once
#include <stddef.h>
#include "safehouse/pager.h"

#define TABLE_MAX_PAGES 100

typedef struct {
    size_t num_rows;
    Pager *pager;
} Table;

typedef struct {
    Table *table;
    size_t row_num;
    bool eof;
} Cursor;

Table *db_open(const char *filename);
void db_close(Table*);
Cursor *table_start(Table *table);
Cursor *table_end(Table *table);
void *cursor_value(Cursor *cursor);
void cursor_advance(Cursor *cursor);