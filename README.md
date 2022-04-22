### Skitter

Collection of my experiments in graphics and games programming

For now, this repo is mostly C code. I am trying out `raylib`
There is also a barebones SDL demo.

Tools assumed to be installed:
- `make`
- `gcc`
- `emsdk`

Download and build `raylib` and use the `libraylib.a` and `libraylibweb.a` for [native](https://github.com/raysan5/raylib/wiki/Working-on-GNU-Linux) and [WASM](https://github.com/raysan5/raylib/wiki/Working-for-Web-(HTML5)) builds respectively.

Generally, I have tried to make the code compile on linux, OSX and WASM. In the `Makefile`, if `target` is native, `target-wasm` and `target-wasm-release` will generally build the same for WASM and `target-osx` will build for OSX

Code:
- Pong (SDL) : `make pong-sdl` and `make pong-sdl-wasm`
- Pong (raylib): `make pong-raylib`
- Game of Life: `make gol`
- [minpx](https://github.com/Datavorous/minpx): `make minpx`