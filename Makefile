test:
	gypkg gen test/test.gyp
	make -C test/out/ -j8
	./test/out/Release/uv_link_t-test

example:
	gypkg gen example/example.gyp
	make -C example/out/ -j8
	./example/out/Release/uv_link_t-example

dist:
	gypkg gen uv_link_t.gyp
	make -C out/ -j8

.PHONY: test example dist
