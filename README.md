<h1 align="center">c_chip-8</h1>

<p align="center">
  <a href="https://github.com/Gabo2447/c_chip-8/blob/main/LICENSE"><picture><source media="(prefers-color-scheme: dark)" srcset="https://shieldcn.dev/github/Gabo2447/c_chip-8/license.svg?size=default" /><img alt="license" src="https://shieldcn.dev/github/Gabo2447/c_chip-8/license.svg?size=default&amp;mode=light" /></picture></a>
  <a href="https://github.com/Gabo2447/c_chip-8/commits"><picture><source media="(prefers-color-scheme: dark)" srcset="https://shieldcn.dev/github/Gabo2447/c_chip-8/last-commit.svg?size=default" /><img alt="last commit" src="https://shieldcn.dev/github/Gabo2447/c_chip-8/last-commit.svg?size=default&amp;mode=light" /></picture></a>
</p>


## Overview

`chip8-emulator` is a lightweight CHIP-8 emulator implemented in C11. It uses SDL2 for real-time graphics rendering and keyboard input mapping, providing a clean execution environment managed via CMake and Ninja.

## Requirements

* C11-compliant C compiler (GCC, Clang)
* CMake 3.10 or higher
* SDL2 development libraries

## Building & Running

1. Clone the repository and configure dependencies via vcpkg:
```bash
git clone https://github.com/Gabo2447/chip8-emulator.git
cd chip8-emulator

```

2. Build the project with CMake:
```bash
cmake -B cmake-build-debug -S . -DCMAKE_TOOLCHAIN_FILE=/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake -G Ninja
cmake --build cmake-build-debug --target all
```

3. Run a ROM file:
```bash
./cmake-build-debug/chip8-emulator "./roms/demos/Maze (alt) [David Winter, 199x].ch8"
```
