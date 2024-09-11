#pragma once

#include <stdio.h>

#define PMT_FATAL_ERROR() abort();

#define PMT_FATAL_ERROR_MSG(MSG_)                                                                                                                                                                                                                              \
 do {                                                                                                                                                                                                                                                          \
  fprintf(stderr, "Fatal error: %s\n", MSG_);                                                                                                                                                                                                                  \
  abort();                                                                                                                                                                                                                                                     \
 } while (0)
