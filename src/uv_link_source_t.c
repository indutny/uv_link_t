#include <stdlib.h>
#include <string.h>

#include "common.h"

typedef struct uv_link_source_write_s uv_link_source_write_t;
typedef struct uv_link_source_shutdown_s uv_link_source_shutdown_t;

struct uv_link_source_write_s {
  uv_write_t req;
  uv_link_t* link;
  uv_link_write_cb write_cb;
};

struct uv_link_source_shutdown_s {
  uv_shutdown_t req;
  uv_link_t* link;
  uv_link_shutdown_cb shutdown_cb;
};


static void uv_link_source_wrap_alloc_cb(uv_handle_t* handle,
                                         size_t suggested_size,
                                         uv_buf_t* buf) {
  uv_link_source_t* source;

  source = handle->data;

  uv_link_propagate_alloc_cb(&source->link, suggested_size, buf);
}


static void uv_link_source_wrap_read_cb(uv_stream_t* stream,
                                        ssize_t nread,
                                        const uv_buf_t* buf) {
  uv_link_source_t* source;

  source = stream->data;

  uv_link_propagate_read_cb(&source->link, nread, buf);
}


static int uv_link_source_read_start(uv_link_t* link) {
  uv_link_source_t* source;

  source = container_of(link, uv_link_source_t, link);

  return uv_read_start(source->stream,
                       uv_link_source_wrap_alloc_cb,
                       uv_link_source_wrap_read_cb);
}


static int uv_link_source_read_stop(uv_link_t* link) {
  uv_link_source_t* source;

  source = container_of(link, uv_link_source_t, link);

  return uv_read_stop(source->stream);
}


static void uv_link_source_wrap_write_cb(uv_write_t* req, int status) {
  uv_link_source_write_t* lreq;

  lreq = container_of(req, uv_link_source_write_t, req);
  lreq->write_cb(lreq->link, status);
  free(lreq);
}


static int uv_link_source_write(uv_link_t* link,
                                const uv_buf_t bufs[],
                                unsigned int nbufs,
                                uv_stream_t* send_handle,
                                uv_link_write_cb cb) {
  uv_link_source_t* source;
  uv_link_source_write_t* req;

  source = container_of(link, uv_link_source_t, link);
  req = malloc(sizeof(*req));
  if (req == NULL)
    return UV_ENOMEM;

  req->link = link;
  req->write_cb = cb;

  return uv_write2(&req->req, source->stream, bufs, nbufs, send_handle,
                   uv_link_source_wrap_write_cb);
}


static int uv_link_source_try_write(uv_link_t* link,
                                    const uv_buf_t bufs[],
                                    unsigned int nbufs) {
  uv_link_source_t* source;

  source = container_of(link, uv_link_source_t, link);

  return uv_try_write(source->stream, bufs, nbufs);
}


static void uv_link_source_wrap_shutdown_cb(uv_shutdown_t* req, int status) {
  uv_link_source_shutdown_t* lreq;

  lreq = container_of(req, uv_link_source_shutdown_t, req);
  lreq->shutdown_cb(lreq->link, status);
  free(lreq);
}


static int uv_link_source_shutdown(uv_link_t* link, uv_link_shutdown_cb cb) {
  uv_link_source_t* source;
  uv_link_source_shutdown_t* req;

  source = container_of(link, uv_link_source_t, link);

  req = malloc(sizeof(*req));
  if (req == NULL)
    return UV_ENOMEM;

  req->link = link;
  req->shutdown_cb = cb;

  return uv_shutdown(&req->req, source->stream,
                     uv_link_source_wrap_shutdown_cb);
}


static uv_link_methods_t uv_link_source_methods = {
  .read_start = uv_link_source_read_start,
  .read_stop = uv_link_source_read_stop,
  .write = uv_link_source_write,
  .try_write = uv_link_source_try_write,
  .shutdown = uv_link_source_shutdown
};


int uv_link_source_init(uv_link_source_t* source,
                        uv_stream_t* stream) {
  int err;

  memset(source, 0, sizeof(*source));

  err = uv_link_init(&source->link, &uv_link_source_methods);
  if (err != 0)
    return err;

  source->stream = stream;
  source->stream->data = source;

  return 0;
}


void uv_link_source_close(uv_link_source_t* source) {
  uv_link_close(&source->link);

  source->stream = NULL;
}
