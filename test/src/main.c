#include <stdio.h>
#include <string.h>

#include "test-list.h"

#define TEST_SELECT(N)                                                        \
    if (strncmp(argv[1], #N, sizeof(#N) - 1) == 0) {                          \
      fprintf(stderr, "===== " #N " =====\n");                                \
      TEST_FN(N)();                                                           \
      return 0;                                                               \
    }

int main(int argc, char** argv) {
  if (argc != 2)
    return -1;

  TEST_ENUM(TEST_SELECT)

  return -1;
}

#undef TEST_SELECT
