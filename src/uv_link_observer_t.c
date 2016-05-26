#include <stdlib.h>
#include <string.h>

#include "src/common.h"


static int uv_link_observer_read_start(uv_link_t* link) {
  uv_link_observer_t* observer;

  observer = container_of(link, uv_link_observer_t, link);

  return uv_link_read_start(observer->target);
}


static int uv_link_observer_read_stop(uv_link_t* link) {
  uv_link_observer_t* observer;

  observer = container_of(link, uv_link_observer_t, link);

  return uv_link_read_stop(observer->target);
}


static int uv_link_observer_write(uv_link_t* link,
                                  uv_link_t* source,
                                  const uv_buf_t bufs[],
                                  unsigned int nbufs,
                                  uv_stream_t* send_handle,
                                  uv_link_write_cb cb) {
  uv_link_observer_t* observer;

  observer = container_of(link, uv_link_observer_t, link);

  return uv_link_write(observer->target, source, bufs, nbufs, send_handle, cb);
}


static int uv_link_observer_try_write(uv_link_t* link,
                                      const uv_buf_t bufs[],
                                      unsigned int nbufs) {
  uv_link_observer_t* observer;

  observer = container_of(link, uv_link_observer_t, link);

  return uv_link_try_write(observer->target, bufs, nbufs);
}


static int uv_link_observer_shutdown(uv_link_t* link,
                                     uv_link_t* source,
                                     uv_link_shutdown_cb cb) {
  uv_link_observer_t* observer;

  observer = container_of(link, uv_link_observer_t, link);

  return uv_link_shutdown(observer->target, source, cb);
}


static void uv_link_observer_alloc_cb(uv_link_t* link,
                                      size_t suggested_size,
                                      uv_buf_t* buf) {
  return uv_link_propagate_alloc_cb(link, suggested_size, buf);
}


static void uv_link_observer_read_cb(uv_link_t* link,
                                     ssize_t nread,
                                     const uv_buf_t* buf) {
  uv_link_observer_t* observer;

  observer = container_of(link, uv_link_observer_t, link);

  if (observer->read_cb != NULL)
    observer->read_cb(observer, nread, buf);

  return uv_link_propagate_read_cb(link, nread, buf);
}


static uv_link_methods_t uv_link_observer_methods = {
  .read_start = uv_link_observer_read_start,
  .read_stop = uv_link_observer_read_stop,
  .write = uv_link_observer_write,
  .try_write = uv_link_observer_try_write,
  .shutdown = uv_link_observer_shutdown,

  .alloc_cb_override = uv_link_observer_alloc_cb,
  .read_cb_override = uv_link_observer_read_cb
};


int uv_link_observer_init(uv_link_observer_t* observer,
                          uv_link_t* target) {
  int err;

  memset(observer, 0, sizeof(*observer));

  err = uv_link_init(&observer->link, &uv_link_observer_methods);
  if (err != 0)
    return err;

  observer->target = target;

  err = uv_link_chain(target, &observer->link);
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
