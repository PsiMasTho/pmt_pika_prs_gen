/*
  EBNF:
    file        = section*;

    section     = "%lib"    ws* (newline identifier_body?)?
                | "%exe"    ws* (newline identifier_body?)?
                | "%custom" ws* (newline identifier_body?)?
                | "%deps"   ws* (newline dependency_body?)?;

    identifier_body = (ws* identifier_stmnt)+;

    dependency_body = (ws* dependency_stmnt)+;

    identifier_stmnt = identifier? ws* eolf;

    dependency_stmnt = (identifier ":" ws+ dependency_list)? ws* eolf;

    dependency_list = identifier (ws identifier)*;

    identifier      = [a-z_];
    ws              = "\s" | "\t";
    eolf            = newline | EOF;
    newline         = "\r"? "\n";
*/

#include "pmt/generator/parser.h"

#include "pmt/generator/ast.h"
#include "pmt/generator/grammar.h"
#include "pmt/base/utility.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CS_IDENTIFIER "abcdefghijklmnopqrstuvwxyz_"

struct parser {
 char* _begin;
 char* _cursor;
 char* _end;
};

static size_t
parser_remaining(parser* self_) {
 return self_->_end - self_->_cursor;
}

bool
parse_eof(parser* self_) {
 return parser_remaining(self_) == 0;
}

bool
parse_newline(parser* self_) {
 if (parse_eof(self_))
  return false;

 if (*self_->_cursor == '\n')
  return ++self_->_cursor, true;

 if (parser_remaining(self_) >= 2 && strncmp(self_->_cursor, "\r\n", 2) == 0) {
  self_->_cursor += 2;
  return true;
 }

 return false;
}

bool
parse_eolf(parser* self_) {
 return parse_eof(self_) || parse_newline(self_);
}

bool
skip_ws(parser* self_) {
 char const* const start = self_->_cursor;

 while (!parse_eof(self_) && strchr(" \t", *self_->_cursor))
  ++self_->_cursor;

 return self_->_cursor != start;
}

bool
parse_identifier(parser* self_, ast_node* ret_) {
 char* backtrack = self_->_cursor;

 if (parse_eof(self_))
  return false;

 while (!parse_eof(self_) && strchr(CS_IDENTIFIER, *self_->_cursor))
  ++self_->_cursor;

 if (self_->_cursor == backtrack) {
  self_->_cursor = backtrack;
  return false;
 }

 ret_->_union._token._size  = self_->_cursor - backtrack;
 ret_->_union._token._begin = pmt_base_strndup(backtrack, ret_->_union._token._size);
 return true;
}

bool
parse_dependency_list(parser* self_, ast_node* ret_) {
 ret_->_union._children = ast_node_children_create();

 while (!parse_eof(self_)) {
  if (ret_->_union._children._size > 0)
   skip_ws(self_);

  ast_node identifier = ast_node_create(GRAMMAR_INFO._entries[GRAMMAR_ID_IDENTIFIER]._tag, GRAMMAR_ID_IDENTIFIER);

  if (!parse_identifier(self_, &identifier))
   break;

  ast_node_children_push(&ret_->_union._children, identifier);
 }

 if (ret_->_union._children._size == 0) {
  ast_node_children_destroy(&ret_->_union._children);
  return false;
 }
 return true;
}

bool
parse_dependency_stmnt(parser* self_, ast_node* ret_) {
 ast_node identifier = ast_node_create(GRAMMAR_INFO._entries[GRAMMAR_ID_IDENTIFIER]._tag, GRAMMAR_ID_IDENTIFIER);
 char*    backtrack  = self_->_cursor;

 ret_->_union._children = ast_node_children_create();

 if (parse_identifier(self_, &identifier))
  if (!parse_eof(self_) && *self_->_cursor == ':') {
   ++self_->_cursor;

   if (skip_ws(self_)) {
    ast_node dependency_list = ast_node_create(GRAMMAR_INFO._entries[GRAMMAR_ID_DEPENDENCY_LIST]._tag, GRAMMAR_ID_DEPENDENCY_LIST);

    if (parse_dependency_list(self_, &dependency_list)) {
     ast_node_children_push(&ret_->_union._children, identifier);
     ast_node_children_push(&ret_->_union._children, dependency_list);
    }
   }
  }

 skip_ws(self_);

 if (!parse_eolf(self_)) {
  ast_node_children_destroy(&ret_->_union._children);
  self_->_cursor = backtrack;
  return false;
 }

 return true;
}

bool
parse_identifier_stmnt(parser* self_, ast_node* ret_) {
 char* backtrack = self_->_cursor;

 ast_node          identifier = ast_node_create(GRAMMAR_INFO._entries[GRAMMAR_ID_IDENTIFIER]._tag, GRAMMAR_ID_IDENTIFIER);
 ast_node_children children   = ast_node_children_create();

 if (parse_identifier(self_, &identifier))
  ast_node_children_push(&children, identifier);

 skip_ws(self_);

 if (!parse_eolf(self_)) {
  ast_node_children_destroy(&ret_->_union._children);
  self_->_cursor = backtrack;
  return false;
 }

 ret_->_union._children = children;
 return true;
}

bool
parse_dependency_body(parser* self_, ast_node* ret_) {
 ret_->_union._children = ast_node_children_create();

 while (true) {
  skip_ws(self_);

  ast_node dependency_stmnt = ast_node_create(GRAMMAR_INFO._entries[GRAMMAR_ID_DEPENDENCY_STMNT]._tag, GRAMMAR_ID_DEPENDENCY_STMNT);

  if (!parse_dependency_stmnt(self_, &dependency_stmnt))
   break;

  ast_node_children_push(&ret_->_union._children, dependency_stmnt);
 }

 if (ret_->_union._children._size == 0) {
  ast_node_children_destroy(&ret_->_union._children);
  return false;
 }

 return true;
}

bool
parse_identifier_body(parser* self_, ast_node* ret_) {
 ret_->_union._children = ast_node_children_create();

 while (!parse_eof(self_)) {
  skip_ws(self_);

  ast_node identifier_stmnt = ast_node_create(GRAMMAR_INFO._entries[GRAMMAR_ID_IDENTIFIER_STMNT]._tag, GRAMMAR_ID_IDENTIFIER_STMNT);

  if (!parse_identifier_stmnt(self_, &identifier_stmnt))
   break;

  ast_node_children_push(&ret_->_union._children, identifier_stmnt);
 }

 if (ret_->_union._children._size == 0) {
  ast_node_children_destroy(&ret_->_union._children);
  return false;
 }
 return true;
}

bool
parse_section(parser* self_, ast_node* ret_) {
 char* backtrack = self_->_cursor;

 if (parse_eof(self_)) {
  self_->_cursor = backtrack;
  return false;
 }

 ret_->_tag = AST_NODE_CHILDREN;

 if (parser_remaining(self_) >= 4 && strncmp(self_->_cursor, "%lib", 4) == 0) {
  self_->_cursor += 4;
  ret_->_id = GRAMMAR_ID_SECTION_LIB;
 } else if (parser_remaining(self_) >= 4 && strncmp(self_->_cursor, "%exe", 4) == 0) {
  self_->_cursor += 4;
  ret_->_id = GRAMMAR_ID_SECTION_EXE;
 } else if (parser_remaining(self_) >= 7 && strncmp(self_->_cursor, "%custom", 7) == 0) {
  self_->_cursor += 7;
  ret_->_id = GRAMMAR_ID_SECTION_CUSTOM;
 } else if (parser_remaining(self_) >= 5 && strncmp(self_->_cursor, "%deps", 5) == 0) {
  self_->_cursor += 5;
  ret_->_id = GRAMMAR_ID_SECTION_DEPS;
 } else {
  self_->_cursor = backtrack;
  return false;
 }

 skip_ws(self_);

 if (!parse_newline(self_))
  return true;

 ast_node body = ast_node_create(GRAMMAR_INFO._entries[ret_->_id]._tag, ret_->_id);

 switch (ret_->_id) {
 case GRAMMAR_ID_SECTION_LIB:
 case GRAMMAR_ID_SECTION_EXE:
 case GRAMMAR_ID_SECTION_CUSTOM:
  if (parse_identifier_body(self_, &body))
   ast_node_children_push(&body._union._children, body);
  else
   ast_node_destroy(&body);
  break;
 case GRAMMAR_ID_SECTION_DEPS:
  if (parse_dependency_body(self_, &body))
   ast_node_children_push(&body._union._children, body);
  else
   ast_node_destroy(&body);
  break;
 default:
  /* Unreachable */
  exit(1);
 }
 return true;
}

bool
parse_file(parser* self_, ast_node* ret_) {
 ret_->_union._children = ast_node_children_create();

 while (true) {
  if (ret_->_union._children._size > 0)
   skip_ws(self_);

  ast_node section = ast_node_create(GRAMMAR_INFO._entries[GRAMMAR_ID_FILE]._tag, GRAMMAR_ID_FILE);

  if (!parse_section(self_, &section))
   break;

  ast_node_children_push(&ret_->_union._children, section);
 }

 if (parser_remaining(self_) > 0) {
  ast_node_children_destroy(&ret_->_union._children);
  return false;
 }

 return true;
}

/* ****************************************************************************
  * PUBLIC *******************************************************************
  *****************************************************************************/

parser*
parser_create(char const* filename_) {
 parser* self = malloc(sizeof(parser));
 FILE*   file = fopen(filename_, "rb");

 size_t read, size = 0, capacity = 4096;

 if (!self) {
  fprintf(stderr, "Failed to allocate memory for parser\n");
  exit(1);
 }

 if (!file) {
  fprintf(stderr, "Failed to open file: %s\n", filename_);
  exit(1);
 }

 self->_begin = malloc(capacity);

 if (!self->_begin) {
  fprintf(stderr, "Failed to allocate memory for parser buffer\n");
  exit(1);
 }

 while (true) {
  if (size == capacity) {
   capacity *= 2;
   self->_begin = realloc(self->_begin, capacity);

   if (!self->_begin) {
    fprintf(stderr, "Failed to reallocate memory for parser buffer\n");
    exit(1);
   }
  }

  read = fread(self->_begin + size, 1, capacity - size, file);

  if (read == 0)
   break;

  size += read;
 }

 self->_cursor = self->_begin;
 self->_end    = self->_begin + size;

 fclose(file);

 return self;
}

void
parser_destroy(parser* self_) {
 pmt_base_free(self_->_begin);
 self_->_begin = self_->_cursor = self_->_end = NULL;
}

ast_node
parser_parse(parser* self_) {
 ast_node ret = ast_node_create(GRAMMAR_INFO._entries[GRAMMAR_ID_FILE]._tag, GRAMMAR_ID_FILE);

 if (!parse_file(self_, &ret))
  ast_node_destroy(&ret);

 return ret;
}
