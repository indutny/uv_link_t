#include <sys/socket.h>
#include <unistd.h>

#include "test-common.h"

static int fds[2];
static uv_loop_t* loop;
static uv_pipe_t pair_right;
static uv_link_source_t source;

static int write_cb_called;
static int alloc_cb_called;
static int read_cb_called;

static void read_one() {
  char buf[1024];
  int err;

  do
    err = read(fds[0], buf, sizeof(buf));
  while (err == -1 && errno == EINTR);

  CHECK_EQ(err, 1, "read() == 1");
  CHECK_EQ(buf[0], 'x', "data should match");
}

static void source_write_cb(uv_link_t* link, int status) {
  CHECK_EQ(link, &source.link, "link == &source.link");

  write_cb_called++;

  read_one();
}


static void test_writes() {
  uv_buf_t buf;

  /* .write() should work */
  buf = uv_buf_init("x", 1);
  CHECK_EQ(source.link.write(&source.link, &buf, 1, NULL, source_write_cb), 0,
           "source.link.write() should return 0");

  CHECK_EQ(uv_run(loop, UV_RUN_DEFAULT), 0, "uv_run()");

  CHECK_EQ(write_cb_called, 1, "source_write_cb() must be called");

  /* .try_write() should work too */
  CHECK_EQ(source.link.try_write(&source.link, &buf, 1), 1,
           "source.link.try_write() should return 1");
  read_one();
}


static void source_alloc_cb(uv_link_t* link,
                            size_t suggested_size,
                            uv_buf_t* buf) {
  static char storage[1024];

  CHECK_EQ(link, &source.link, "link == &source.link");

  alloc_cb_called++;

  buf->base = storage;
  buf->len = sizeof(storage);
}

static void source_read_cb(uv_link_t* link,
                           ssize_t nread,
                           const uv_buf_t* buf) {
  CHECK_EQ(link, &source.link, "link == &source.link");

  read_cb_called++;

  CHECK_EQ(nread, 1, "source_read_cb must read one byte");
  CHECK_EQ(buf->base[0], 'x', "source_read_cb must read correct one byte");

  CHECK_EQ(source.link.read_stop(&source.link), 0, "source.link.read_stop()");
}


static void test_reads() {
  int err;

  /* alloc_cb/read_cb */
  source.link.alloc_cb = source_alloc_cb;
  source.link.read_cb = source_read_cb;

  do
    err = write(fds[0], "x", 1);
  while (err == -1 && errno == EINTR);
  CHECK_EQ(err, 1, "write() == 1");

  CHECK_EQ(source.link.read_start(&source.link), 0, "source.link.read_start()");

  CHECK_EQ(uv_run(loop, UV_RUN_DEFAULT), 0, "uv_run()");
  CHECK_EQ(alloc_cb_called, 1, "alloc_cb must be called once");
  CHECK_EQ(read_cb_called, 1, "read_cb must be called once");
}


TEST_IMPL(uv_link_source_t) {
  loop = uv_default_loop();
  CHECK_NE(loop, NULL, "uv_default_loop()");

  CHECK_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, fds), 0, "socketpair()");

  CHECK_EQ(uv_pipe_init(loop, &pair_right, 0), 0, "uv_pipe_init(pair_right)");
  CHECK_EQ(uv_pipe_open(&pair_right, fds[1]), 0, "uv_pipe_open(pair_right)");

  CHECK_EQ(uv_link_source_init(loop, &source, (uv_stream_t*) &pair_right), 0,
           "uv_link_source_init()");

  test_writes();
  test_reads();

  uv_close((uv_handle_t*) &pair_right, NULL);

  CHECK_EQ(close(fds[0]), 0, "close(fds[0])");
}
