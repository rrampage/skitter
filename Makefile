.PHONY: clean pong-raylib gol
clean:
	rm ./target/*
sdl_flags := $(shell sdl2-config --cflags --libs)
linux_flags := -lGL -lm -lpthread -ldl -lrt
osx_flags := -framework OpenGL -framework Cocoa -framework IOKit -lm -lpthread -ldl
c_flags := -Wall -Wextra -std=c99
wasm_flags := -sUSE_GLFW=3 -sSINGLE_FILE
wasm_release_flags := -Oz --closure 1
pong-sdl: pong-sdl/pong.c
	gcc -o target/pong $(c_flags) pong-sdl/pong.c $(sdl_flags) && ./target/pong
pong-sdl-wasm: pong-sdl/pong.c
	emcc -o target/pong_sdl.html pong-sdl/pong.c $(c_flags) -sUSE_SDL=2
pong-raylib: pong-raylib/pong.c
	gcc -o target/pong-raylib $(c_flags) pong-raylib/pong.c lib/libraylib.a -I ./include $(linux_flags) && ./target/pong-raylib
pong-raylib-osx: pong-raylib/pong.c
	gcc -o target/pong-raylib $(c_flags) pong-raylib/pong.c lib/libraylib.a -I ./include $(osx_flags) && ./target/pong-raylib
pong-raylib-wasm: pong-raylib/pong.c
	emcc -o target/pong.html $(c_flags) pong-raylib/pong.c lib/libraylibweb.a -I ./include $(wasm_flags) $(wasm_release_flags) -sFILESYSTEM=0 --shell-file pong-raylib/shell.html
gol: game-of-life/game_of_life.c
	gcc -o target/gol $(c_flags) game-of-life/*.c lib/libraylib.a -I ./include $(linux_flags) && ./target/gol
gol-osx: game-of-life/game_of_life.c
	gcc -o target/gol $(c_flags) game-of-life/*.c lib/libraylib.a -I ./include $(osx_flags) && ./target/gol
gol-wasm: game-of-life/game_of_life.c
	emcc -o target/gol.html $(c_flags) game-of-life/*.c lib/libraylibweb.a -I ./include $(wasm_flags)
gol-wasm-release: game-of-life/game_of_life.c
	emcc -o target/gol.html $(c_flags) game-of-life/*.c lib/libraylibweb.a -I ./include $(wasm_flags) $(wasm_release_flags) -sFILESYSTEM=0 --shell-file game-of-life/shell.html
minpx: minpx/*.c
	gcc -o target/minpx $(c_flags) minpx/*.c lib/libraylib.a -I ./include $(linux_flags) && ./target/minpx
minpx-osx: minpx/*.c
	gcc -o target/minpx $(c_flags) minpx/*.c lib/libraylib.a -I ./include $(osx_flags) && ./target/minpx
minpx-wasm: minpx/main.c
	emcc -o target/minpx.html $(c_flags) minpx/*.c lib/libraylibweb.a  -I ./include $(wasm_flags) --shell-file minpx/shell.html --preload-file minpx/assets --preload-file storage.data
minpx-wasm-release: minpx/main.c
	emcc -o target/minpx.html $(c_flags) minpx/*.c lib/libraylibweb.a  -I ./include $(wasm_flags) $(wasm_release_flags) --shell-file minpx/shell.html --preload-file minpx/assets --preload-file minpx/storage.data
