### Skitter

Collection of my experiments in graphics and games programming

For now, this repo is mostly C code. I am trying out `raylib`
I also have a barebones SDL demo.

Tools assumed to be installed:
- `make`
- `gcc`
- `emsdk`

Download and build `raylib` and use the `libraylib.a` and `libraylibweb.a` for native and WASM builds respectively.

Generally, I have tried to make the code compile to both linux and WASM. In the `Makefile`, if `target` is native, `target-wasm` and `target-wasm-release` will generally build the same for WASM.

Code:
- Pong (SDL) : `make pong-sdl` and `make pong-sdl-wasm`
- Pong (raylib): `make pong-raylib`
- Game of Life: `make gol`
- [minpx](https://github.com/Datavorous/minpx): `make minpx`