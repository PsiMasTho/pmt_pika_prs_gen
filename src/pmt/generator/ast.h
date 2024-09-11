#pragma once

#include <stddef.h>

typedef struct ast_token {
 char*  _begin;
 size_t _size;
} ast_token;

typedef struct ast_node_children {
 struct ast_node* _data;
 size_t           _size;
 size_t           _capacity;
} ast_node_children;

typedef union ast_node_union {
 ast_token         _token;
 ast_node_children _children;
} ast_node_union;

typedef enum ast_node_tag {
 AST_NODE_TOKEN,
 AST_NODE_CHILDREN,
} ast_node_tag;

typedef struct ast_node {
 ast_node_union _union;
 ast_node_tag   _tag;
 int            _id;
} ast_node;

ast_token
ast_token_create(char* begin_, size_t size_);

void
ast_token_destroy(ast_token* self_);

ast_node_children
ast_node_children_create(void);

void
ast_node_children_destroy(ast_node_children* self_);

void
ast_node_children_push(ast_node_children* self_, ast_node node_);

ast_node
ast_node_create(ast_node_tag tag_, int id_);

void
ast_node_destroy(ast_node* self_);
