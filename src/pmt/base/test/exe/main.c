#include <stdio.h>

// clang-format off
#ifdef NDEBUG
  #warning "The unit test module must be compiled without NDEBUG defined."
#endif
// clang-frmat on

void pmt_base_opaque_vector_test(void);
void pmt_base_opaque_map_test(void);
void pmt_base_bitset_test(void);

int main(void)
{
    pmt_base_opaque_vector_test();
    pmt_base_opaque_map_test();
    pmt_base_bitset_test();
    printf("No assertions triggered. All tests passed.\n");
    return 0;
}
