{
  "targets": [{
    "target_name": "uv_link_t",
    "type": "<!(gypkg type)",

    "direct_dependent_settings": {
      "include_dirs": [ "include" ],
    },

    "variables": {
      "gypkg_deps": [
        "git://github.com/libuv/libuv.git@^1.9.0:uv.gyp:libuv",
      ],
    },

    "dependencies": [
      "<!@(gypkg deps <(gypkg_deps))"
    ],

    "include_dirs": [
      ".",
    ],

    "sources": [
      "src/defaults.c",
      "src/uv_link_t.c",
      "src/uv_link_observer_t.c",
      "src/uv_link_source_t.c",
    ],
  }],
}
