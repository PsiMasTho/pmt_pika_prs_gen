#pragma once

#include "ast.h"

typedef enum grammar_id {
 GRAMMAR_ID_FILE,
 GRAMMAR_ID_SECTION_LIB,
 GRAMMAR_ID_SECTION_EXE,
 GRAMMAR_ID_SECTION_CUSTOM,
 GRAMMAR_ID_SECTION_DEPS,
 GRAMMAR_ID_IDENTIFIER_STMNT,
 GRAMMAR_ID_DEPENDENCY_STMNT,
 GRAMMAR_ID_IDENTIFIER_BODY,
 GRAMMAR_ID_DEPENDENCY_BODY,
 GRAMMAR_ID_DEPENDENCY_LIST,
 GRAMMAR_ID_IDENTIFIER,
 COUNT_GRAMMAR_ID,
} grammar_id;

typedef struct grammar_info_entry {
 char const*  _name;
 ast_node_tag _tag;
} grammar_info_entry;

typedef struct grammar_info {
 grammar_info_entry _entries[COUNT_GRAMMAR_ID];
} grammar_info;

extern grammar_info const GRAMMAR_INFO;
