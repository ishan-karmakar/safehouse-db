#include <string.h>
#include <stdio.h>
#include "safehouse/row.h"

void row_serialize(Row *source, void *destination) {
    memcpy(destination + ID_OFFSET, &(source->id), ID_SIZE);
    strncpy(destination + USERNAME_OFFSET, source->username, USERNAME_SIZE);
    strncpy(destination + EMAIL_OFFSET, source->email, EMAIL_SIZE);
}

void row_deserialize(void *source, Row *destination) {
    memcpy(&(destination->id), source + ID_OFFSET, ID_SIZE);
    memcpy(&(destination->username), source + USERNAME_OFFSET, USERNAME_SIZE);
    memcpy(&(destination->email), source + EMAIL_OFFSET, EMAIL_SIZE);
}

void row_print(Row *row) { printf("(%ld, %s, %s)\n", row->id, row->username, row->email); }