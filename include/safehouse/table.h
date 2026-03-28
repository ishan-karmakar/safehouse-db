#pragma once
#include <stddef.h>
#include "safehouse/pager.h"

#define TABLE_MAX_PAGES 100

typedef struct {
    size_t num_rows;
    Pager *pager;
} Table;

Table *db_open(const char *filename);
void db_close(Table*);
void *table_row_slot(Table *table, size_t row_num);