#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "test-list.h"

/* TODO(indutny): TAP */

#define TEST_SELECT(N)                                                        \
    if (strncmp(argv[1], #N, sizeof(#N) - 1) == 0) {                          \
      fprintf(stderr, "===== " #N " =====\n");                                \
      TEST_FN(N)();                                                           \
      return 0;                                                               \
    }

/* TODO(indutny): fork and run */

#define TEST_RUN(N)                                                           \
    do {                                                                      \
      fprintf(stderr, "===== " #N " =====\n");                                \
      TEST_FN(N)();                                                           \
    } while (0);

int main(int argc, char** argv) {
  if (argc == 2) {
    TEST_ENUM(TEST_SELECT)

    return -1;
  }

  TEST_ENUM(TEST_RUN)

  return 0;
}

#undef TEST_SELECT
