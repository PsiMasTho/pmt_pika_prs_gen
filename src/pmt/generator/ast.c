#include "ast.h"

#include <stdlib.h>
#include <string.h>

#define AST_MIN_CAPACITY 4

static size_t
get_next_capacity(size_t current_capacity_) {
 return current_capacity_ < AST_MIN_CAPACITY ? AST_MIN_CAPACITY : current_capacity_ * 2;
}

static void
ast_node_children_grow(ast_node_children* self_) {
 self_->_capacity = get_next_capacity(self_->_capacity);
 self_->_data     = realloc(self_->_data, self_->_capacity * sizeof(ast_node));
}

ast_token
ast_token_create(char* begin_, size_t size_) {
 ast_token self;

 self._begin = malloc(size_);
 self._size  = size_;

 strncpy(self._begin, begin_, size_);
 return self;
}

void
ast_token_destroy(ast_token* self_) {
 free(self_->_begin);
 self_->_begin = NULL;
 self_->_size  = 0;
}

ast_node_children
ast_node_children_create(void) {
 ast_node_children self;

 self._capacity = get_next_capacity(0);
 self._size     = 0;
 self._data     = malloc(self._capacity * sizeof(ast_node));
 return self;
}

void
ast_node_children_destroy(ast_node_children* self_) {
 for (size_t i = 0; i < self_->_size; ++i)
  ast_node_destroy(&self_->_data[i]);

 free(self_->_data);
 self_->_data = NULL;
 self_->_size = 0;
}

void
ast_node_children_push(ast_node_children* self_, ast_node node_) {
 if (self_->_size == self_->_capacity)
  ast_node_children_grow(self_);
 self_->_data[self_->_size++] = node_;
}

ast_node
ast_node_create(ast_node_tag tag_, int id_) {
 ast_node self;

 self._tag = tag_;
 self._id  = id_;
 return self;
}

void
ast_node_destroy(ast_node* self_) {
 if (self_->_tag == AST_NODE_CHILDREN)
  ast_node_children_destroy(&self_->_union._children);
 else if (self_->_tag == AST_NODE_TOKEN)
  ast_token_destroy(&self_->_union._token);
}
