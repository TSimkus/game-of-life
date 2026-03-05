[Conway's Game Of Life](https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life) in C using [SDL3](https://www.libsdl.org/)

## Quick start

```
# Clone with submodules, otherwise SDL will not be downloaded
git clone https://github.com/TSimkus/game-of-life --depth=1 --recurse-submodules

cd game-of-life

cmake -S . -B build

cmake --build build

./build/game-of-life
```
