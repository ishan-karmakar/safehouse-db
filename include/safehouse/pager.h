#pragma once
#include <stddef.h>

#define TABLE_MAX_PAGES 100

static const size_t PAGE_SIZE = 0x1000;

typedef struct {
    int fd;
    size_t file_length;
    void *pages[TABLE_MAX_PAGES];
} Pager;

Pager *pager_open(const char *filename);
void *pager_get(Pager *pager, size_t page_num);
void pager_flush(Pager *pager, size_t page_num, size_t size);
