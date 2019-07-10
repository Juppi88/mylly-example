# Mylly Example Game

This repository contains a simple Asteroids-like game demonstrating the features of Mylly Game Engine.

## Building

Building the demo (as well as the engine itself) is managed with [CMake](https://cmake.org/). The project should compile with at least the following compilers:

* MSVC 2017 (Windows)
* Clang 6.0 (Linux)
* GCC 5.4 (Linux)

Currently building on Linux will require installing the engine dependencies manually (see the list in the engine repository readme). By default CMake generates a Makefile to build the game and the engine.

On Windows, CMake generates a Visual Studio solution to build the game. Dependency libraries are included within the engine repository, so only OpenGL development files are required to be installed.

On both platforms CMake will copy game resources to the output directory.

### Example (on Linux)

```
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make
```
