#ifndef TEST_SRC_TEST_LIST_H_
#define TEST_SRC_TEST_LIST_H_

#define TEST_ENUM(V)                                                          \
    V(uv_link_source_t)                                                       \
    V(uv_link_observer_t)                                                     \

#define TEST_DECL(N) void test__##N();

TEST_ENUM(TEST_DECL)

#undef TEST_DECL

#define TEST_FN(N) test__##N
#define TEST_IMPL(N) void test__##N()

#endif  /* TEST_SRC_TEST_LIST_H_ */
