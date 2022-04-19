.PHONY: clean
clean:
	rm ./target/*
sdl_flags := $(shell sdl2-config --cflags --libs)
pong-sdl: pong-sdl/pong.c
	gcc -o target/pong -Wall -std=c99 pong-sdl/pong.c $(sdl_flags) && ./target/pong
pong-sdl-wasm: pong-sdl/pong.c
	emcc -o target/pong_sdl.html pong-sdl/pong.c -Wall -std=c99 -sUSE_SDL=2
