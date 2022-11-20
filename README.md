# dots-and-boxes

Simple Dots and Boxes game implementation using Min-Max algorithm.

## Build instructions
This project uses vcpkg as a dependency manager if the app is built for the system. Otherwise, emscripten is used.

### Vcpkg
Install necesarry libraries (freetype, SDL2, GLEW, opengl, imgui, glm) and run:

```
cmake -S . -B build-host/ -DCMAKE_TOOLCHAIN_FILE=~[VCPKG_ROOT]/scripts/buildsystems/vcpkg.cmake
```

### Emscripten

To build the app for web, run the following command:
```
emcmake cmake -S . -B build-emscripten/
```