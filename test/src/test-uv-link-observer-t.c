#include <sys/socket.h>
#include <unistd.h>

#include "test-common.h"

static uv_loop_t* loop;
static uv_link_t source;
static uv_link_observer_t observer;

static int observer_read_cb_called;

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

  loop = uv_default_loop();
  CHECK_NE(loop, NULL, "uv_default_loop()");

  CHECK_EQ(uv_link_init(loop, &source), 0, "uv_link_init(source)");

  CHECK_EQ(uv_link_observer_init(loop, &observer, &source), 0,
           "uv_link_observer_init()");

  observer.read_cb = observer_read_cb;

  uv_link_invoke_alloc_cb(&source, 1024, &buf);

  buf.base[0] = 'x';
  uv_link_invoke_read_cb(&source, 1, &buf);
  CHECK_EQ(observer_read_cb_called, 1, "observer.read_cb must be called");

  CHECK_EQ(uv_link_observer_close(&observer), 0, "uv_link_observer_close");
  uv_link_close(&source);
}
