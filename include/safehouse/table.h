#pragma once
#include <stdint.h>

#define TABLE_MAX_PAGES 100

typedef struct {
    uint32_t num_rows;
    void *pages[TABLE_MAX_PAGES];
} Table;

Table *table_create(void);
void table_destroy(Table*);
void *table_row_slot(Table *table, uint32_t row_num);