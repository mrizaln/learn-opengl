# learn-opengl

Code and notes I took while learning OpenGL at [learnopengl.com](https://learnopengl.com) (again)

**I decided to start over my learning process from the beginning.**

Old repo: [learn-opengl-old](https://github.com/mrizaln/learn-opengl-old)

## Dependencies

- GCC 13 (or other compiler with C++20 support)
- CMake (3.10+)
- Conan 2.0
- [GLFW](https://github.com/glfw/glfw)
- [stb](https://github.com/nothings/stb) (stb_image specifically)
- [GLM](https://github.com/g-truc/glm)
- [glbinding](https://github.com/cginternals/glbinding) (included in repository as submodule)
- [ImGui](https://github.com/ocornut/imgui) (included in repository as submodule)
- [assimp](https://github.com/assimp/assimp)

Library dependencies are managed using Conan.

## Building

Every chapter has its own code directory and is a project on its own. To build a project, first you need to navigate to that project root directory (with CMakeLists.txt).

> For example: Hello Triangle chapter is in directory `main/1_getting_started/1.2_hello_triangle/code/`

Then you need to install the dependencies using Conan:

```sh
conan install . -of build/debug/ --build missing -s build_type=Debug
```

> You can change the `build_type` from `Debug` to `Release` if you want (also you can change the output directory as well: `... -of <output_directory> ...`)

After the dependencies are installed, the building process is straightforward:

```sh
cmake --preset conan-debug
cmake --build --preset conan-debug
```

> Note that if you changed the `build_type` to `Release` at the previous step, the preset name is changed to `conan-release`. Make sure to reflect that in the commmand.
