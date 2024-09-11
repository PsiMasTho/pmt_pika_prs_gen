#ifndef PMT_BASE_OPAQUE_HANDLE_H
#define PMT_BASE_OPAQUE_HANDLE_H

#include <stddef.h>

#define DEFINE_OPAQUE_HANDLE(N)                                                                                                                                                                                                                                \
 typedef struct pmt_base_opaque_handle_##N {                                                                                                                                                                                                                   \
  void* _opaque_data[N];                                                                                                                                                                                                                                       \
 } pmt_base_opaque_handle_##N;

DEFINE_OPAQUE_HANDLE(1)
DEFINE_OPAQUE_HANDLE(2)
DEFINE_OPAQUE_HANDLE(3)
DEFINE_OPAQUE_HANDLE(4)
DEFINE_OPAQUE_HANDLE(5)
DEFINE_OPAQUE_HANDLE(6)
DEFINE_OPAQUE_HANDLE(7)
DEFINE_OPAQUE_HANDLE(8)
DEFINE_OPAQUE_HANDLE(9)
DEFINE_OPAQUE_HANDLE(10)
DEFINE_OPAQUE_HANDLE(11)
DEFINE_OPAQUE_HANDLE(12)
DEFINE_OPAQUE_HANDLE(13)
DEFINE_OPAQUE_HANDLE(14)
DEFINE_OPAQUE_HANDLE(15)
DEFINE_OPAQUE_HANDLE(16)

#endif //PMT_BASE_OPAQUE_HANDLE_H
