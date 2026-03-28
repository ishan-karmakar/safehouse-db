#pragma once
#include <stddef.h>
#include <stdint.h>
#include "safehouse/pager.h"
#include "safehouse/row.h"

typedef enum {
    NODE_INTERNAL,
    NODE_LEAF
} NodeType;

const size_t NODE_TYPE_SIZE = sizeof(uint8_t);
const size_t NODE_TYPE_OFFSET = 0;
const size_t IS_ROOT_SIZE = sizeof(uint8_t);
const size_t IS_ROOT_OFFSET = NODE_TYPE_OFFSET + IS_ROOT_SIZE;
const size_t PARENT_POINTER_SIZE = sizeof(size_t);
const size_t PARENT_POINTER_OFFSET = IS_ROOT_OFFSET + PARENT_POINTER_SIZE;
const size_t COMMON_NODE_HEADER_SIZE = NODE_TYPE_SIZE + IS_ROOT_SIZE + PARENT_POINTER_SIZE;

const size_t LEAF_NODE_NUM_CELLS_SIZE = sizeof(size_t);
const size_t LEAF_NODE_NUM_CELLS_OFFSET = COMMON_NODE_HEADER_SIZE;
const size_t LEAF_NODE_HEADER_SIZE = COMMON_NODE_HEADER_SIZE + LEAF_NODE_NUM_CELLS_SIZE;

const size_t LEAF_NODE_KEY_SIZE = sizeof(size_t);
const size_t LEAF_NODE_KEY_OFFSET = 0;
const size_t LEAF_NODE_VALUE_SIZE = ROW_SIZE;
const size_t LEAF_NODE_VALUE_OFFSET = LEAF_NODE_KEY_OFFSET + LEAF_NODE_KEY_SIZE;
const size_t LEAF_NODE_CELL_SIZE = LEAF_NODE_KEY_SIZE + LEAF_NODE_VALUE_SIZE;
const size_t LEAF_NODE_SPACE_FOR_CELLS = PAGE_SIZE - LEAF_NODE_HEADER_SIZE;
const size_t LEAF_NODE_MAX_CELLS = LEAF_NODE_SPACE_FOR_CELLS / LEAF_NODE_CELL_SIZE;

static size_t *leaf_node_num_cells(void *n) { return (size_t*) (n + LEAF_NODE_NUM_CELLS_OFFSET); }
static void *leaf_node_cell(void *n, size_t num) { return n + LEAF_NODE_HEADER_SIZE + num * LEAF_NODE_CELL_SIZE; }
static size_t *leaf_node_key(void *n, size_t num) { return (size_t*) leaf_node_cell(n, num); }
static void *leaf_node_value(void *n, size_t num) { return leaf_node_cell(n, num) + LEAF_NODE_KEY_SIZE; }
static void initialize_leaf_node(void *n) { *leaf_node_num_cells(n) = 0; }
