#include <stdlib.h>
#include <string.h>

#include "src/common.h"


static int uv_link_observer_read_start(uv_link_t* link) {
  return uv_link_read_start(link->parent);
}


static int uv_link_observer_read_stop(uv_link_t* link) {
  return uv_link_read_stop(link->parent);
}


static int uv_link_observer_write(uv_link_t* link,
                                  uv_link_t* source,
                                  const uv_buf_t bufs[],
                                  unsigned int nbufs,
                                  uv_stream_t* send_handle,
                                  uv_link_write_cb cb,
                                  void* arg) {
  return uv_link_propagate_write(link->parent, source, bufs, nbufs,
                                 send_handle, cb, arg);
}


static int uv_link_observer_try_write(uv_link_t* link,
                                      const uv_buf_t bufs[],
                                      unsigned int nbufs) {
  return uv_link_try_write(link->parent, bufs, nbufs);
}


static int uv_link_observer_shutdown(uv_link_t* link,
                                     uv_link_t* source,
                                     uv_link_shutdown_cb cb,
                                     void* arg) {
  return uv_link_propagate_shutdown(link->parent, source, cb, arg);
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

  observer = (uv_link_observer_t*) link;

  if (observer->observer_read_cb != NULL)
    observer->observer_read_cb(observer, nread, buf);

  return uv_link_propagate_read_cb(link, nread, buf);
}


void uv_link_observer_close(uv_link_t* link, uv_link_t* source,
                            uv_link_close_cb cb) {
  cb(source);
}


static uv_link_methods_t uv_link_observer_methods = {
  .read_start = uv_link_observer_read_start,
  .read_stop = uv_link_observer_read_stop,
  .write = uv_link_observer_write,
  .try_write = uv_link_observer_try_write,
  .shutdown = uv_link_observer_shutdown,
  .close = uv_link_observer_close,

  .alloc_cb_override = uv_link_observer_alloc_cb,
  .read_cb_override = uv_link_observer_read_cb
};


int uv_link_observer_init(uv_link_observer_t* observer) {
  int err;

  observer->read_cb = NULL;

  err = uv_link_init((uv_link_t*) observer, &uv_link_observer_methods);
  if (err != 0)
    return err;

  return 0;
}
