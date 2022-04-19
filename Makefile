.PHONY: clean
clean:
	rm ./target/*
sdl_flags := $(shell sdl2-config --cflags --libs)
pong-sdl: pong-sdl/pong.c
	gcc -o target/pong -Wall -std=c99 pong-sdl/pong.c $(sdl_flags) && ./target/pong
pong-sdl-wasm: pong-sdl/pong.c
	emcc -o target/pong_sdl.html pong-sdl/pong.c -Wall -std=c99 -sUSE_SDL=2
pong-raylib: pong-raylib/pong.c
	gcc -o target/pong-raylib -Wall -std=c99 pong-raylib/pong.c -I ./include lib/libraylib.a -lGL -lm -lpthread -ldl -lrt && ./target/pong-raylib
pong-raylib-wasm: pong-raylib/pong.c
	emcc -o target/pong_raylib.html -Wall -Wextra -std=c99 pong-raylib/pong.c -I ./include -Oz --closure 1 lib/libraylibweb.a -sUSE_GLFW=3 -sFILESYSTEM=0 -sSINGLE_FILE --shell-file pong-raylib/shell.html
