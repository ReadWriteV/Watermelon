# Watermelon

游戏 **合成大西瓜** 的C++版本，使用到了 **SDL2** 和 **box2d** 库。

![screenshot](./doc/image/screenshot.png)

## Prerequisites

+ A C++17 compatible compiler
+ SDL2 (2.26.2)
+ SDL2_image (2.6.2)
+ SDL2_mixer (2.6.2)
+ box2d (2.4.1)

Other version may work with minor modification

## Build

Run `cmake` in the root directory to compile the sources.

``` bash

cd Watermelon

cmake -S . -B build

cmake --build build
```
