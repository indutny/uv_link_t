#include <sys/socket.h>
#include <unistd.h>

#include "test-common.h"

static uv_link_t st_link;

static int close_cb_called;


const char* test_strerror(uv_link_t* l, int err) {
  CHECK_EQ(l, &st_link, "link == st_link");
  return "Description";
}


static uv_link_methods_t methods = {
  .strerror = test_strerror,
  .close = uv_link_default_close
};

static void close_cb(uv_link_t* l) {
  close_cb_called++;
}


TEST_IMPL(strerror) {
  CHECK_EQ(uv_link_init(&st_link, &methods), 0, "uv_link_init()");

  CHECK_EQ(strcmp(uv_link_strerror(&st_link, -1), "Description"), 0,
           "error description should match");

  uv_link_close(&st_link, close_cb);
  CHECK_EQ(close_cb_called, 1, "close_cb must be called");
}
