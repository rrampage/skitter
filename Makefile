.PHONY: clean pong-raylib gol
clean:
	rm ./target/*
sdl_flags := $(shell sdl2-config --cflags --libs)
pong-sdl: pong-sdl/pong.c
	gcc -o target/pong -Wall -std=c99 pong-sdl/pong.c $(sdl_flags) && ./target/pong
pong-sdl-wasm: pong-sdl/pong.c
	emcc -o target/pong_sdl.html pong-sdl/pong.c -Wall -std=c99 -sUSE_SDL=2
pong-raylib: pong-raylib/pong.c
	gcc -o target/pong-raylib -Wall -std=c99 pong-raylib/pong.c lib/libraylib.a -I ./include -lGL -lm -lpthread -ldl -lrt && ./target/pong-raylib
pong-raylib-wasm: pong-raylib/pong.c
	emcc -o target/pong_raylib.html -Wall -Wextra -std=c99 pong-raylib/pong.c -I ./include -Oz --closure 1 lib/libraylibweb.a -sUSE_GLFW=3 -sFILESYSTEM=0 -sSINGLE_FILE --shell-file pong-raylib/shell.html
gol: game-of-life/game_of_life.c
	gcc -o target/gol -Wall -std=c99 game-of-life/*.c lib/libraylib.a -I ./include -lGL -lm -lpthread -ldl -lrt && ./target/gol
gol-wasm: game-of-life/game_of_life.c
	emcc -o target/gol.html -Wall -Wextra -std=c99 game-of-life/*.c lib/libraylibweb.a -I ./include -sUSE_GLFW=3
gol-wasm-release: game-of-life/game_of_life.c
	emcc -o target/gol.html -Wall -Wextra -std=c99 game-of-life/*.c lib/libraylibweb.a -I ./include -Oz --closure 1 -sUSE_GLFW=3 -sFILESYSTEM=0 -sSINGLE_FILE --shell-file game-of-life/shell.html
minpx: minpx/main.c
	gcc -o target/minpx -Wall -std=c99 minpx/*.c lib/libraylib.a -I ./include -lGL -lm -lpthread -ldl -lrt && ./target/minpx
minpx-wasm: minpx/main.c
	emcc -o target/minpx.html -Wall -std=c99 minpx/*.c lib/libraylibweb.a  -I ./include -sUSE_GLFW=3 --shell-file minpx/shell.html --preload-file minpx/assets --preload-file storage.data
minpx-wasm-release: minpx/main.c
	emcc -o target/minpx.html -Wall -std=c99 -Oz --closure 1 minpx/*.c lib/libraylibweb.a  -I ./include -sUSE_GLFW=3 -sSINGLE_FILE --shell-file minpx/shell.html --preload-file minpx/assets --preload-file storage.data