# uv_link_t

## Types

### uv_link_t

The base unit of all APIs. Pointer to this structure is what needs to be passed
to the most of the methods.

### uv_link_methods_t

TODO(indutny)

### uv_link_source_t

TODO(indutny)

### uv_link_observer_t

TODO(indutny)

## Methods

### uv_link_t

#### int uv_link_init(...)
#### void uv_link_close(...)
#### int uv_link_chain(...)
#### int uv_link_unchain(...)
#### void uv_link_propagate_alloc_cb(...)
#### void uv_link_propagate_read_cb(...)
#### int uv_link_propagate_write(...)
#### int uv_link_propagate_shutdown(...)
#### void uv_link_propagate_close(...)
#### int uv_link_read_start(...)
#### int uv_link_read_stop(...)
#### int uv_link_write(...)
#### int uv_link_try_write(...)
#### int uv_link_shutdown(...)

### uv_link_source_t

#### int uv_link_source_init(...)
#### int uv_link_observer_init(...)
