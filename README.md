# learn-opengl

Code and notes I took while learning OpenGL at [learnopengl.com](https://learnopengl.com) (again)

**I decided to start over my learning process from the beginning.**

Old repo: [learn-opengl-old](https://github.com/mrizaln/learn-opengl-old)

## Dependencies

- GCC 13 (or other compiler with C++20 support)
- CMake (3.10+)
- [GLFW](https://github.com/glfw/glfw)
- [glbinding](https://github.com/cginternals/glbinding) (included in repository as submodule)

### Linux

- Fedora

  `$ sudo dnf install glfw glfw-devel`

- Others

  Use your distro package manager. The package name may be different, but it will have similar name. Make sure to install both the binary package and the development package just like shown above.

### Windows

Idk, download the dependencies manually I guess.

`¯\_(ツ)_/¯`

## Compiling

Every chapter has its own code directory. Create a build directory inside that directory.

> For example: `main/1_getting_started/1.2_hello_triangle/code/`

```sh
$ cd main/1_getting_started/1.2_hello_triangle/code/
$ mkdir build && cd build
```

Then, run the cmake command below:

```sh
$ cmake ..
$ cmake --build . -j$(nproc)
```
