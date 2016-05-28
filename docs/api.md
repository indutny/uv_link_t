# uv_link_t

## General notes

All methods with `int` return value return `0` on success or error code, unless
specified otherwise.

## uv_link_t

The base unit of all APIs. Pointer to this structure is what needs to be passed
to the most of the methods.

### int uv_link_init(...)

* `uv_link_t* link` - user allocated `uv_link_t` instance
* `uv_link_methods_t const* methods` - user defined link methods, see
  [uv_link_methods][] below

Initialize `link` structure with provided `methods`.

This is a first method to be called on `uv_link_t` instance before it could be
used in any other APIs.

### void uv_link_close(...)

* `uv_link_t* link` - `uv_link_t` instance to close
* `uv_link_close_cb cb` - callback to be invoked upon close

Close `uv_link_t` instance and all other `uv_link_t`s chained into it (see
[uv_link_chain][]), invoke `void (*cb)(uv_link_t*)` once done (may happen
synchronously or on a next libuv tick, depending on particular `uv_link_t`
implementation).

Invokes `close` from link's [method table][].

*NOTE: this method must be called on a leaf link that has no chaining child*

### int uv_link_chain(...)

* `uv_link_t* from`
* `uv_link_t* to`

"Chain" two `uv_link_t` instances together, set `from.child = to` and
`to.parent = from`. After this call, all data emitted by `from` link via
[uv_link_propagate_alloc_cb][] and [uv_link_propagate_read_cb][] will be passed
to `to` link's [method table][]'s [alloc_cb_override][] and
[read_cb_override][].

Closing `to` with `uv_link_close` will also close `from`, unless they will be
later unchained with [uv_link_unchain][].

*NOTE: Links can have at most one parent and one child.*

### int uv_link_unchain(...)

* `uv_link_t* from`
* `uv_link_t* to`

Unlink chains, restoring `from.read_cb` and `from.alloc_cb` to their previous
values (that they had before `uv_link_unchain()` call).

### int uv_link_read_start(...)

* `uv_link_t* link`

Invoke `read_start` from the link's [method table][]. [alloc_cb][]/[read_cb][]
may be called after successful `uv_link_read_start`.

### int uv_link_read_stop(...)

* `uv_link_t* link`

Invoke `read_stop` from the link's [method table][]. [alloc_cb][]/[read_cb][]
won't be called after `uv_link_read_stop`.

### int uv_link_write(...)

* `uv_link_t* link`
* `const uv_buf_t bufs[]` - buffers to write
* `unsigned int nbufs` - number of buffers to write
* `uv_stream_t* send_handle` - handle to send through IPC (if supported by
  underlying [uv_link_source_t][])
* `uv_link_write_cb cb` - callback to be invoked
* `void* arg` - user data to be passed to callback

Invoke `write` from the link's [method table][]. Acts similarly to `uv_write()`.
`cb(uv_link_t* link, int status, void* arg)` is invoked on completion.

### int uv_link_try_write(...)

* `uv_link_t* link`
* `const uv_buf_t bufs[]` - buffers to write
* `unsigned int nbufs` - number of buffers to write

Invoke `try_write` from the link's [method table][]. Acts similarly to
`uv_try_write()`.

Returns number of bytes written, or negative error code.

### int uv_link_shutdown(...)

* `uv_link_t* link`
* `uv_link_shutdown_cb cb` - callback to be invoked
* `void* arg` - user data to be passed to callback

Invoke `shutdown` from the link's [method table][]. Acts similarly to
`uv_shutdown()`. `cb(uv_link_t* link, int status, void* arg)` is invoked on
completion.

### void uv_link_propagate_alloc_cb(...)

Should be used only by `uv_link_methods_t` implementation.

* `uv_link_t* link`
* `size_t suggested_size`
* `uv_buf_t* buf`

Arguments have the same meaning as in `uv_alloc_cb`. When calling this function
`link->alloc_cb` will be invoked with either `link` or `link->child` (if latter
is not `NULL`).

This should be used to emit data from `uv_link_t`. Semantics are the same as of
`uv_alloc_cb`.

### void uv_link_propagate_read_cb(...)

Should be used only by `uv_link_methods_t` implementation.

* `uv_link_t* link`
* `ssize_t nread`
* `const uv_buf_t* buf`

Arguments have the same meaning as in `uv_read_cb`. When calling this function
`link->read_cb` will be invoked with either `link` or `link->child` (if latter
is not `NULL`).

This should be used to emit data from `uv_link_t`. Semantics are the same as of
`uv_read_cb`.

### int uv_link_propagate_write(...)

Should be used only by `uv_link_methods_t` implementation.

### int uv_link_propagate_shutdown(...)

Should be used only by `uv_link_methods_t` implementation.

### void uv_link_propagate_close(...)

Should be used only by `uv_link_methods_t` implementation.

### .data

User data property of `void*` type, any value may be stored here.

### .alloc_cb
### .read_cb
### .parent
### .child

## uv_link_methods_t

## uv_link_source_t

### int uv_link_source_init(...)
### int uv_link_observer_init(...)

## uv_link_observer_t

### int uv_link_observer_init(...)
### .observer_read_cb
