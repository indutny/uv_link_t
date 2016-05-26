#ifndef INCLUDE_UV_LINK_H_
#define INCLUDE_UV_LINK_H_

#include "uv.h"

typedef struct uv_link_s uv_link_t;
typedef struct uv_link_methods_s uv_link_methods_t;
typedef struct uv_link_source_s uv_link_source_t;
typedef struct uv_link_observer_s uv_link_observer_t;

typedef void (*uv_link_alloc_cb)(uv_link_t* link,
                                 size_t suggested_size,
                                 uv_buf_t* buf);
typedef void (*uv_link_read_cb)(uv_link_t* link,
                                ssize_t nread,
                                const uv_buf_t* buf);
typedef void (*uv_link_write_cb)(uv_link_t* source, int status);
typedef void (*uv_link_shutdown_cb)(uv_link_t* source, int status);

struct uv_link_methods_s {
  int (*read_start)(uv_link_t* link);
  int (*read_stop)(uv_link_t* link);

  int (*write)(uv_link_t* link,
               uv_link_t* source,
               const uv_buf_t bufs[],
               unsigned int nbufs,
               uv_stream_t* send_handle,
               uv_link_write_cb cb);
  int (*try_write)(uv_link_t* link,
                   const uv_buf_t bufs[],
                   unsigned int nbufs);

  int (*shutdown)(uv_link_t* link, uv_link_t* source, uv_link_shutdown_cb cb);

  /* Overriding callbacks */
  uv_link_alloc_cb alloc_cb_override;
  uv_link_read_cb read_cb_override;
};

struct uv_link_s {
  uv_link_t* parent;
  uv_link_t* child;

  uv_link_alloc_cb alloc_cb;
  uv_link_read_cb read_cb;

  void* data;

  /* Read-only after assigning initial values */

  /* Sort of virtual table */
  uv_link_methods_t const* methods;

  /* Private, used for chain/unchain */
  uv_link_alloc_cb saved_alloc_cb;
  uv_link_read_cb saved_read_cb;
};

UV_EXTERN int uv_link_init(uv_link_t* link, uv_link_methods_t const* methods);
UV_EXTERN void uv_link_close(uv_link_t* link);

UV_EXTERN int uv_link_chain(uv_link_t* from, uv_link_t* to);
UV_EXTERN int uv_link_unchain(uv_link_t* from, uv_link_t* to);

/* Use this to invoke parent link's methods */
UV_EXTERN void uv_link_propagate_alloc_cb(uv_link_t* link,
                                          size_t suggested_size,
                                          uv_buf_t* buf);
UV_EXTERN void uv_link_propagate_read_cb(uv_link_t* link,
                                         ssize_t nread,
                                         const uv_buf_t* buf);

/* Use this to invoke methods of `link` */

static int uv_link_read_start(uv_link_t* link) {
  return link->methods->read_start(link);
}

static int uv_link_read_stop(uv_link_t* link) {
  return link->methods->read_stop(link);
}

static int uv_link_write(uv_link_t* link,
                         uv_link_t* source,
                         const uv_buf_t bufs[],
                         unsigned int nbufs,
                         uv_stream_t* send_handle,
                         uv_link_write_cb cb) {
  return link->methods->write(link, source, bufs, nbufs, send_handle, cb);
}


static int uv_link_try_write(uv_link_t* link,
                             const uv_buf_t bufs[],
                             unsigned int nbufs) {
  return link->methods->try_write(link, bufs, nbufs);
}

static int uv_link_shutdown(uv_link_t* link,
                            uv_link_t* source,
                            uv_link_shutdown_cb cb) {
  return link->methods->shutdown(link, source, cb);
}

/* Link Source */

struct uv_link_source_s {
  uv_link_t link;

  uv_stream_t* stream;
};

/* NOTE: uses `stream->data` field */
UV_EXTERN int uv_link_source_init(uv_link_source_t* source,
                                  uv_stream_t* stream);
UV_EXTERN void uv_link_source_close(uv_link_source_t* source);

/* Link Observer */

struct uv_link_observer_s {
  uv_link_t link;
  uv_link_t* target;

  /* This will be called, even if the ones in `link` will be overwritten */
  void (*read_cb)(uv_link_observer_t* observer,
                  ssize_t nread,
                  const uv_buf_t* buf);
};

UV_EXTERN int uv_link_observer_init(uv_link_observer_t* observer,
                                    uv_link_t* target);
UV_EXTERN int uv_link_observer_close(uv_link_observer_t* observer);

#endif  /* INCLUDE_UV_LINK_H_ */
