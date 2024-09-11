#include "pmt/base/args.h"

#include "pmt/base/opaque_vector.h"
#include "pmt/base/utility.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct arg_and_value {
 pmt_base_arg_info  _arg;
 pmt_base_arg_value _value;
} arg_and_value;

typedef struct pmt_base_args_parser_internal {
 pmt_base_opaque_vector _data; /* arg_and_value */
 bool                   _parsed;
} pmt_base_args_parser_internal;

typedef union pmt_base_args_parser_union {
 pmt_base_args_parser_internal _internal;
 pmt_base_args_parser          _handle;
} pmt_base_opaque_map_union;

static pmt_base_arg_info
pmt_base_arg_info_deep_copy(pmt_base_arg_info self_) {
 pmt_base_arg_info ret;
 ret._description = pmt_base_strdup(self_._description);
 ret._long        = pmt_base_strdup(self_._long);
 ret._tag         = self_._tag;
 ret._short       = self_._short;
 return ret;
}

static void
pmt_base_arg_info_destroy(pmt_base_arg_info* self_) {
 free(self_->_description);
 free(self_->_long);
}

static bool
args_parser_find_short(pmt_base_args_parser_internal* self_, char arg_, size_t* index_) {
 for (size_t i = 0; i < self_->_args_size; ++i)
  if (self_->_args[i]._short == arg_) {
   *index_ = i;
   return true;
  }

 return false;
}

static bool
args_parser_find_long(pmt_base_args_parser_internal* self_, char const* arg_, size_t* index_) {
 for (size_t i = 0; i < self_->_args_size; ++i)
  if (self_->_args[i]._long && strcmp(self_->_args[i]._long, arg_) == 0) {
   *index_ = i;
   return true;
  }

 return false;
}

static size_t
args_parser_set_value(pmt_base_args_parser* self_, size_t index_, char const* arg_, char const* next_) {
 if (self_->_values[index_]._was_present) {
  fprintf(stderr, "Duplicate argument: %s\n", self_->_args[index_]._long);
  exit(1);
 }

 self_->_values[index_]._was_present = true;
 self_->_values[index_]._type        = self_->_args[index_]._type;
 switch (self_->_values[index_]._type) {
 case ARGS_TYPE_STRING:
  if (!next_) {
   fprintf(stderr, "Expected argument for: %s\n", arg_);
   exit(1);
  }
  self_->_values[index_]._union._string = str_dup(next_);
  break;
 case ARGS_TYPE_INT:
  if (!next_) {
   fprintf(stderr, "Expected argument for: %s\n", arg_);
   exit(1);
  }
  self_->_values[index_]._union._int = atoi(next_);
  break;
 default:
  break;
 }

 return self_->_values[index_]._type == ARGS_TYPE_VOID ? 0 : 1;
}

pmt_base_args_parser*
args_parser_create(void) {
 pmt_base_args_parser* self = malloc(sizeof(pmt_base_args_parser));

 if (!self) {
  fprintf(stderr, "Failed to allocate memory for pmt_base_args_parser\n");
  exit(1);
 }

 self->_args_size     = 0;
 self->_args_capacity = get_next_capacity(0);
 self->_args          = malloc(sizeof(pmt_base_arg_info) * self->_args_capacity);
 self->_values        = NULL;
 self->_parsed        = false;

 if (!self->_args) {
  fprintf(stderr, "Failed to allocate memory for pmt_base_args_parser args\n");
  exit(1);
 }

 return self;
}

void
args_parser_destroy(pmt_base_args_parser* self_) {
 /* Destroy args */
 for (size_t i = 0; i < self_->_args_size; ++i)
  pmt_base_arg_info_destroy(&self_->_args[i]);
 free(self_->_args);

 /* Destroy values */
 if (self_->_parsed) {
  for (size_t i = 0; i < self_->_args_size; ++i) {
   if (self_->_values[i]._was_present && self_->_values[i]._type == ARGS_TYPE_STRING) {
    free(self_->_values[i]._union._string);
   }
  }

  free(self_->_values);
 }

 /* Destroy self */
 free(self_);
}

void
args_parser_add_arg(pmt_base_args_parser* self_, pmt_base_arg_info pmt_base_arg_info_) {
 if (self_->_parsed) {
  fprintf(stderr, "Cannot add arguments after parsing\n");
  exit(1);
 }

 if (self_->_args_size == self_->_args_capacity) {
  self_->_args_capacity = get_next_capacity(self_->_args_capacity);
  self_->_args          = realloc(self_->_args, sizeof(pmt_base_arg_info) * self_->_args_capacity);

  if (!self_->_args) {
   fprintf(stderr, "Failed to reallocate memory for pmt_base_args_parser args\n");
   exit(1);
  }
 }

 self_->_args[self_->_args_size++] = pmt_base_arg_info_;
}

void
args_parser_parse(pmt_base_args_parser* self_, int argc_, char const* const* argv_) {
 size_t argc = (size_t)argc_;

 self_->_values = malloc(sizeof(pmt_base_arg_value) * self_->_args_size);

 if (!self_->_values) {
  fprintf(stderr, "Failed to allocate memory for pmt_base_args_parser values\n");
  exit(1);
 }

 // Initialize values
 for (size_t i = 0; i < self_->_args_size; ++i) {
  self_->_values[i]._was_present = false;
  self_->_values[i]._type        = self_->_args[i]._type;
 }

 for (size_t i = 0; i < argc; ++i) {
  if (argv_[i][0] == '-') {
   if (argv_[i][1] == '-') {
    /* long */
    size_t index;
    if (!args_parser_find_long(self_, argv_[i] + 2, &index)) {
     fprintf(stderr, "Unknown option: %s\n", argv_[i]);
     exit(1);
    }
    i += args_parser_set_value(self_, index, argv_[i], argv_[i + 1]);
   } else if (argv_[i][1] != '\0') {
    /* short */
    if (argv_[i][2] != '\0') {
     fprintf(stderr, "Expected short option, got: %s\n", argv_[i]);
     exit(1);
    }

    size_t index;
    if (!args_parser_find_short(self_, argv_[i][1], &index)) {
     fprintf(stderr, "Unknown option: %s\n", argv_[i]);
     exit(1);
    }
    i += args_parser_set_value(self_, index, argv_[i], argv_[i + 1]);
   } else {
    fprintf(stderr, "Invalid argument: %s\n", argv_[i]);
    exit(1);
   }
  }
 }

 self_->_parsed = true;
}

pmt_base_arg_value
args_parser_get_long(pmt_base_args_parser* self_, char const* arg_) {
 size_t index;
 if (!args_parser_find_long(self_, arg_, &index)) {
  fprintf(stderr, "Unknown option: %s\n", arg_);
  exit(1);
 }

 return self_->_values[index];
}

pmt_base_arg_value
args_parser_get_short(pmt_base_args_parser* self_, char arg_) {
 size_t index;
 if (!args_parser_find_short(self_, arg_, &index)) {
  fprintf(stderr, "Unknown option: %c\n", arg_);
  exit(1);
 }

 return self_->_values[index];
}

void
args_parser_print_args(pmt_base_args_parser* self_) {
 printf("Options:\n");

 for (size_t i = 0; i < self_->_args_size; ++i)
  printf("  -%c, --%s: %s\n", self_->_args[i]._short, self_->_args[i]._long, self_->_args[i]._description);
}
