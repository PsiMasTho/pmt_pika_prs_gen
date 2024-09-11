#pragma once

#include "pmt/base/opaque_handle.h"

#include <stdbool.h>

enum {
 PMT_BASE_ARGS_TAG_STRING,
 PMT_BASE_ARGS_TAG_INT,
 PMT_BASE_ARGS_TAG_VOID,
};

typedef union pmt_base_arg_union {
 char* _string;
 int   _int;
} pmt_base_arg_union;

typedef struct pmt_base_arg_value {
 pmt_base_arg_union _union;
 int                _tag;
 bool               _was_present;
} pmt_base_arg_value;

typedef struct pmt_base_arg_info {
 char* _description;
 char* _long;  /* NULL if none, not including leading -- */
 int   _tag;   /* one of PMT_BASE_ARGS_TAG */
 char  _short; /* 0 if none */
} pmt_base_arg_info;

typedef struct pmt_base_opaque_handle_6 pmt_base_args_parser;

pmt_base_args_parser*
pmt_base_args_parser_create(void);

void
pmt_base_args_parser_destroy(pmt_base_args_parser* self_);

void
pmt_base_args_parser_add_arg(pmt_base_args_parser* self_, pmt_base_arg_info pmt_base_arg_info_);

void
pmt_base_args_parser_parse(pmt_base_args_parser* self_, int argc_, char const* const* argv_);

pmt_base_arg_value
pmt_base_args_parser_get_long(pmt_base_args_parser* self_, char const* arg_);

pmt_base_arg_value
pmt_base_args_parser_get_short(pmt_base_args_parser* self_, char arg_);

void
pmt_base_args_parser_print_args(pmt_base_args_parser* self_);
