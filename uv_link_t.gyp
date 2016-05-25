{
  "targets": [{
    "target_name": "uv_link_t",
    "type": "<(library)",

    "direct_dependent_settings": {
      "include_dirs": [ "include" ],
    },
    "include_dirs": [
      # libuv
      "<(uv_dir)/include",

      "include",
      "src",
    ],

    "sources": [
      "src/uv_link.c",
      "src/uv_link_source.c",
    ],
  }],
}
