#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "ast.h"

typedef struct parser parser;

parser* parser_create(char const* filename_);

void parser_destroy(parser* self_);

ast_node parser_parse(parser* self_);
