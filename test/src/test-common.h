#ifndef TEST_SRC_TEST_COMMON_H_
#define TEST_SRC_TEST_COMMON_H_

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "uv.h"
#include "uv_link_t.h"

#include "test-list.h"

#define CHECK(VALUE, MESSAGE)                                                \
    do {                                                                     \
      if ((VALUE)) break;                                                    \
      fprintf(stderr, "Assertion failure: " #MESSAGE "\n");                  \
      abort();                                                               \
    } while (0)

#define CHECK_EQ(A, B, MESSAGE) CHECK((A) == (B), MESSAGE)
#define CHECK_NE(A, B, MESSAGE) CHECK((A) != (B), MESSAGE)

#endif  /* TEST_SRC_TEST_COMMON_H_ */
