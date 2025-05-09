# PBR-Renderer

A framework for physically based rendering using the Cook-Torrance BRDF model and image-based lighting

## Screenshots

## Build

### Windows
```
git clone --recursive https://github.com/lanwenzhang/PBR-Renderer.git
cd PBR-Renderer
mkdir build
cd build
cmake -G "Visual Studio 17 2022" -A x64 ..
```

### Dependencies
* [glfw](https://github.com/glfw/glfw)
* [glad](https://glad.dav1d.de/)
* [imgui](https://github.com/ocornut/imgui)
* [stb](https://github.com/nothings/stb)
* [tinyexr](https://github.com/syoyo/tinyexr)
* [assimp](https://github.com/assimp/assimp)
