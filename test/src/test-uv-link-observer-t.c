#include <sys/socket.h>
#include <unistd.h>

#include "test-common.h"

static uv_link_t source;
static uv_link_observer_t observer;

static int observer_read_cb_called;

static uv_link_methods_t methods = {
  /* no-op, won't be called */
};

static void observer_read_cb(uv_link_observer_t* o,
                             ssize_t nread,
                             const uv_buf_t* buf) {
  CHECK_EQ(o, &observer, "o == &observer");
  CHECK_EQ(nread, 1, "nread == 1");
  CHECK_EQ(buf->base[0], 'x', "correct buf contents");

  observer_read_cb_called++;
}


TEST_IMPL(uv_link_observer_t) {
  uv_buf_t buf;

  CHECK_EQ(uv_link_init(&source, &methods), 0, "uv_link_init(source)");

  CHECK_EQ(uv_link_observer_init(&observer, &source), 0,
           "uv_link_observer_init()");

  observer.read_cb = observer_read_cb;

  uv_link_invoke_alloc_cb(&source, 1024, &buf);

  buf.base[0] = 'x';
  uv_link_invoke_read_cb(&source, 1, &buf);
  CHECK_EQ(observer_read_cb_called, 1, "observer.read_cb must be called");

  CHECK_EQ(uv_link_observer_close(&observer), 0, "uv_link_observer_close");
  uv_link_close(&source);
}
