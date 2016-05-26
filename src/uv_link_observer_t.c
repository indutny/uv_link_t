#include <stdlib.h>
#include <string.h>

#include "common.h"


static int uv_link_observer_read_start(uv_link_t* link) {
  uv_link_observer_t* observer;

  observer = container_of(link, uv_link_observer_t, link);

  return observer->target->read_start(observer->target);
}


static int uv_link_observer_read_stop(uv_link_t* link) {
  uv_link_observer_t* observer;

  observer = container_of(link, uv_link_observer_t, link);

  return observer->target->read_stop(observer->target);
}


static int uv_link_observer_write(uv_link_t* link,
                                  const uv_buf_t bufs[],
                                  unsigned int nbufs,
                                  uv_stream_t* send_handle,
                                  uv_link_write_cb cb) {
  uv_link_observer_t* observer;

  observer = container_of(link, uv_link_observer_t, link);

  return observer->target->write(observer->target, bufs, nbufs, send_handle, cb);
}


static int uv_link_observer_try_write(uv_link_t* link,
                                      const uv_buf_t bufs[],
                                      unsigned int nbufs) {
  uv_link_observer_t* observer;

  observer = container_of(link, uv_link_observer_t, link);

  return observer->target->try_write(observer->target, bufs, nbufs);
}


static int uv_link_observer_shutdown(uv_link_t* link, uv_link_shutdown_cb cb) {
  uv_link_observer_t* observer;

  observer = container_of(link, uv_link_observer_t, link);

  return observer->target->shutdown(observer->target, cb);
}


static void uv_link_observer_alloc_cb(uv_link_t* link,
                                      size_t suggested_size,
                                      uv_buf_t* buf) {
  return uv_link_invoke_alloc_cb(link, suggested_size, buf);
}


static void uv_link_observer_read_cb(uv_link_t* link,
                                     ssize_t nread,
                                     const uv_buf_t* buf) {
  uv_link_observer_t* observer;

  observer = container_of(link, uv_link_observer_t, link);

  if (observer->read_cb != NULL)
    observer->read_cb(observer, nread, buf);

  return uv_link_invoke_read_cb(link, nread, buf);
}


int uv_link_observer_init(uv_loop_t* loop,
                          uv_link_observer_t* observer,
                          uv_link_t* target) {
  int err;
  uv_link_t* l;

  memset(observer, 0, sizeof(*observer));

  err = uv_link_init(loop, &observer->link);
  if (err != 0)
    return err;

  observer->target = target;

  l = &observer->link;
  l->read_start = uv_link_observer_read_start;
  l->read_stop = uv_link_observer_read_stop;
  l->write = uv_link_observer_write;
  l->try_write = uv_link_observer_try_write;
  l->shutdown = uv_link_observer_shutdown;

  err = uv_link_chain(target, l, uv_link_observer_alloc_cb,
                      uv_link_observer_read_cb);
  if (err != 0) {
    uv_link_close(&observer->link);
    return err;
  }

  return 0;
}


int uv_link_observer_close(uv_link_observer_t* observer) {
  int err;

  err = uv_link_unchain(observer->target, &observer->link);
  if (err != 0)
    return err;

  uv_link_close(&observer->link);

  observer->target = NULL;

  return 0;
}
