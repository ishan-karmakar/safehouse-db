#pragma once
#include <stdint.h>

#define COLUMN_USERNAME_SIZE 32
#define COLUMN_EMAIL_SIZE 255

typedef struct {
    uint32_t id;
    char username[COLUMN_USERNAME_SIZE];
    char email[COLUMN_EMAIL_SIZE];
} Row;

#define ATTR_SIZE(Struct, Attribute) sizeof(((Struct*)0)->Attribute)
static const size_t ID_SIZE = ATTR_SIZE(Row, id);
static const size_t USERNAME_SIZE = ATTR_SIZE(Row, username);
static const size_t EMAIL_SIZE = ATTR_SIZE(Row, email);
#undef ATTR_SIZE

static const size_t ID_OFFSET = 0;
static const size_t USERNAME_OFFSET = ID_OFFSET + ID_SIZE;
static const size_t EMAIL_OFFSET = USERNAME_OFFSET + USERNAME_SIZE;
static const uint32_t ROW_SIZE = ID_SIZE + USERNAME_SIZE + EMAIL_SIZE;

static const uint32_t PAGE_SIZE = 4096;
static const uint32_t ROWS_PER_PAGE = PAGE_SIZE / ROW_SIZE;

void row_serialize(Row *source, void *destination);
void row_deserialize(void *source, Row *destination);
void row_print(Row *row);