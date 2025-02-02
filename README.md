# Snake Game

Basic implementation of the classic Snake Game in C using SDL library.

![](./demo.gif)

### Quick Start

```console
./build.sh
./snake
```

If the build scripts fails to execute with undefined reference to sdl, make sure to
provide the correct path to sdl pkgconfig within `./build.sh` and run it again.

```console
export PKG_CONFIG_PATH=path/to/sdl/lib/pkgconfig/
```

### Controls

- Move snake with `h, j ,k ,l`
- Press `SPACE` to pause
- Press `ESCAPE` to quit
- Press `a` to enter ai mode

### Features & Updates

- Snake wraps around when goes off screen
- [ADD] Automatically restarts after game over
- [ADD] Color skin for Snake
- [FIX] Apple would spawn within Snake's body
- [UPDATE] Changed all drawing functions from `SDL_Surface` to `SDL_Renderer`
- [ADD] Now Snake can play itself! Although being very naive approach it is fun to watch!
