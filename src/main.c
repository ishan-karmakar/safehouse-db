#include <stdlib.h>
#include <sys/types.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stumpless.h>
#include "safehouse/table.h"
#include "safehouse/row.h"
#include "safehouse/node.h"

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

void print_constants(void) {
    printf("Constants:\n");
    printf("ROW_SIZE: %ld\n", ROW_SIZE);
    printf("COMMON_NODE_HEADER_SIZE: %ld\n", COMMON_NODE_HEADER_SIZE);
    printf("LEAF_NODE_HEADER_SIZE: %ld\n", LEAF_NODE_HEADER_SIZE);
    printf("LEAF_NODE_CELL_SIZE: %ld\n", LEAF_NODE_CELL_SIZE);
    printf("LEAF_NODE_SPACE_FOR_CELLS: %d\n", LEAF_NODE_SPACE_FOR_CELLS);
    printf("LEAF_NODE_MAX_CELLS: %d\n", LEAF_NODE_MAX_CELLS);
}

void print_leaf_node(void *node) {
    size_t num_cells = *leaf_node_num_cells(node);
    printf("leaf (size %d)\n", num_cells);
    for (size_t i = 0; i < num_cells; i++) {
        size_t key = *leaf_node_key(node, i);
        printf("  - %d : %d\n", i, key);
    }
}

ExecuteResult execute_insert(Statement *statement, Table *table) {
    void *node = pager_get(table->pager, table->root_page_num);
    if (*leaf_node_num_cells(node) >= LEAF_NODE_MAX_CELLS)
        return EXECUTE_TABLE_FULL;
    
    Row *row_to_insert = &(statement->row_to_insert);
    Cursor *cursor = table_end(table);
    leaf_node_insert(cursor, row_to_insert->id, row_to_insert);
    free(cursor);
    return EXECUTE_SUCCESS;
}

ExecuteResult execute_select(Statement *statement, Table *table) {
    Row row;
    Cursor *cursor = table_start(table);
    while (!cursor->eof) {
        row_deserialize(cursor_value(cursor), &row);
        row_print(&row);
        cursor_advance(cursor);
    }
    return EXECUTE_SUCCESS;
}

MetaCommandResult do_meta_command(InputBuffer *buffer, Table *table) {
    if (strcmp(buffer->buffer, ".exit") == 0) {
        db_close(table);
        exit(EXIT_SUCCESS);
    } else if (strcmp(buffer->buffer, ".btree") == 0) {
        printf("Tree:\n");
        print_leaf_node(pager_get(table->pager, 0));
        return META_COMMAND_SUCCESS;
    } else if (strcmp(buffer->buffer, ".constants") == 0)  {
        print_constants();
        return META_COMMAND_SUCCESS;
    } else return META_COMMAND_UNRECOGNIZED_COMMAND;
}

PrepareResult prepare_statement(InputBuffer *buffer, Statement *statement) {
    if (strncmp(buffer->buffer, "insert", 6) == 0) {
        statement->type = STATEMENT_INSERT;
        int args_assigned = sscanf(buffer->buffer, "insert %ld %s %s", &(statement->row_to_insert.id), statement->row_to_insert.username, statement->row_to_insert.email);
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
    if (argc < 2) {
        stump_c("Must supply a database filename.");
        exit(EXIT_FAILURE);
    }
    char *filename = argv[1];
    Table *table = db_open(filename);
    while (true) {
        print_prompt();
        read_input(input_buffer);

        if (input_buffer->buffer[0] == '.') {
            switch (do_meta_command(input_buffer, table)) {
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