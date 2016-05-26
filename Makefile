test:
	./gyp_uv_link test -Duv_dir=./test/deps/libuv
	make -C out/ -j8
	./out/Release/uv_link_t-test

.PHONY: test
