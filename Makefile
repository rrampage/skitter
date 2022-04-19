.PHONY: clean
clean:
	rm ./target/*
sdl_flags := $(shell sdl2-config --cflags --libs)
pong-sdl: pong-sdl/pong.c
	gcc -o target/pong -Wall -std=c99 pong-sdl/pong.c $(sdl_flags) && ./target/pong
