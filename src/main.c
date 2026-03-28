#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stumpless.h>
#include "safehouse/table.h"
#include "safehouse/row.h"

typedef enum {
    META_COMMAND_SUCCESS,
    META_COMMAND_UNRECOGNIZED_COMMAND
} MetaCommandResult;

typedef struct {
    char *buffer;
    size_t buffer_length;
    ssize_t input_length;
} InputBuffer;

typedef enum {
    PREPARE_SUCCESS,
    PREPARE_SYNTAX_ERROR,
    PREPARE_UNRECOGNIZED_STATEMENT
} PrepareResult;

typedef enum {
    EXECUTE_SUCCESS,
    EXECUTE_TABLE_FULL
} ExecuteResult;

typedef enum {
    STATEMENT_INSERT,
    STATEMENT_SELECT
} StatementType;

typedef struct {
    StatementType type;
    Row row_to_insert;
} Statement;

const uint32_t TABLE_MAX_ROWS = ROWS_PER_PAGE * TABLE_MAX_PAGES;

ExecuteResult execute_insert(Statement *statement, Table *table) {
    if (table->num_rows >= TABLE_MAX_ROWS)
        return EXECUTE_TABLE_FULL;
    
    Row *row_to_insert = &(statement->row_to_insert);
    row_serialize(row_to_insert, table_row_slot(table, table->num_rows));
    table->num_rows++;
    return EXECUTE_SUCCESS;
}

ExecuteResult execute_select(Statement *statement, Table *table) {
    Row row;
    for (uint32_t i = 0; i < table->num_rows; i++) {
        row_deserialize(table_row_slot(table, i), &row);
        row_print(&row);
    }
    return EXECUTE_SUCCESS;
}

MetaCommandResult do_meta_command(InputBuffer *buffer) {
    if (strcmp(buffer->buffer, ".exit") == 0)
        exit(EXIT_SUCCESS);
    else return META_COMMAND_UNRECOGNIZED_COMMAND;
}

PrepareResult prepare_statement(InputBuffer *buffer, Statement *statement) {
    if (strncmp(buffer->buffer, "insert", 6) == 0) {
        statement->type = STATEMENT_INSERT;
        int args_assigned = sscanf(buffer->buffer, "insert %d %s %s", &(statement->row_to_insert.id), statement->row_to_insert.username, statement->row_to_insert.email);
        if (args_assigned < 3)
            return PREPARE_SYNTAX_ERROR;
    } else if (strcmp(buffer->buffer, "select") == 0)
        statement->type = STATEMENT_SELECT;
    else return PREPARE_UNRECOGNIZED_STATEMENT;
    return PREPARE_SUCCESS;
}

ExecuteResult execute_statement(Statement *statement, Table *table) {
    switch (statement->type) {
        case STATEMENT_INSERT:
            return execute_insert(statement, table);
        case STATEMENT_SELECT:
            return execute_select(statement, table);
    }
}

InputBuffer *new_input_buffer(void) {
    InputBuffer* input_buffer = (InputBuffer*) malloc(sizeof(InputBuffer));
    input_buffer->buffer = NULL;
    input_buffer->buffer_length = 0;
    input_buffer->input_length = 0;

    return input_buffer;
}

void print_prompt(void) { printf("db > "); }

void read_input(InputBuffer *input_buffer) {
    ssize_t bytes_read = getline(&(input_buffer->buffer), &(input_buffer->buffer_length), stdin);
    if (bytes_read <= 0) {
        stump_w("Error reading input");
        exit(EXIT_FAILURE);
    }

    input_buffer->input_length = bytes_read - 1;
    input_buffer->buffer[bytes_read - 1] = 0;
}

void close_input_buffer(InputBuffer *buffer) {
    free(buffer->buffer);
    free(buffer);
}

int main(int argc, char *argv[]) {
    stumpless_open_stream_target("stdout", stdout);
    InputBuffer *input_buffer = new_input_buffer();
    Table *table = table_create();
    while (true) {
        print_prompt();
        read_input(input_buffer);

        if (input_buffer->buffer[0] == '.') {
            switch (do_meta_command(input_buffer)) {
                case META_COMMAND_SUCCESS: continue;
                case META_COMMAND_UNRECOGNIZED_COMMAND:
                    stump_w("Unrecognized command '%s'", input_buffer->buffer);
                    continue;
            }
        }

        Statement statement;
        switch (prepare_statement(input_buffer, &statement)) {
            case PREPARE_SUCCESS:
                break;
            case PREPARE_SYNTAX_ERROR:
                stump_w("Syntax error. Could not parse statement.");
                continue;
            case PREPARE_UNRECOGNIZED_STATEMENT:
                stump_w("Unrecognized keyword at start of '%s'", input_buffer->buffer);
                continue;
        }
        switch (execute_statement(&statement, table)) {
            case EXECUTE_SUCCESS:
                stump_i("Executed.");
                break;
            case EXECUTE_TABLE_FULL:
                stump_c("Error: Table full.");
                break;
        }
    }
}