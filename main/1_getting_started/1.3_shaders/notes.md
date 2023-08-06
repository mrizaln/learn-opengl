# Shaders

Shaders are litter program that rest on the GPU. These programs are run for each specific section of the graphics pipeline.

## GLSL

Shaders are written in the C-like language GLSL. Shaders always begin with a version declaration, followed by a list of input and output variables, uniforms and its `main` function. Each shader's entry point is at its `main` function.

A shader typically has the following structure:

```glsl
in type in_variable_name;
in type in_variable_name;

out type out_variable_name;

uniform type uniform_name;

void main()
{
    // process input(s) and do some weird graphics stuff
    // ...

    // output processed stuff to output variable
    out_variable_name = weird_stuff_we_processed;
}
```

When we're talking specifically about the vertex shader, each input variable is also known as **`vertex attribute`**. There is a maximum number of vertex attributes we're allowed to declare limited by the hardware. OpenGL guarantees there are always at least 16 4-component vertex attributes available, but some hardware may allow for more which you can retrieve by querying `GL_MAX_VERTEX_ATTRIBS`

```cpp
int nrAttributes;
glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
std::cout << "Maximum number of vertex attributes supported: " << nrAttributes << '\n';
```

### Types

GLSL has most of the default basic types we know from languages like C: `int`, `float`, `double`, `uint`, and `bool`. GLSL also features two container types that we'll be using a lot: `vectors` and `matrices`.

#### Vectors

A vectoa GLSL is a 2, 3, or 4 component container for any of the basic types just mentioned (n represents the number of components):

- `vecn`: the default vector of `n` floats
- `bvecn`: a vector of `n` booleans
- `ivecn`: a vector of `n` integers
- `uvecn`: a vector of `n` unsigned integers
- `dvecn`: a vector of `n` double components

Components of a vector can be accessed via `vec.x` where `x` is the first component of the vector. You can use `.x`, `.y`, `.z`, and `.w` to access the first, second, third, and fourth component respectively. GLSL also allows you to use `rgba` for colors of `stpq` for texture coordinates, accessing the same components.

The vector data type allows us for some interesting and flexible component selection called **`swizzling`**. Swizzling allows us to use syntax like this:

```glsl
vec2 someVec      = vec2(0.8, 0.1);
vec4 differentVec = someVec.xyxx;
vec3 anotherVec   = differentVec.zyw;
vec4 otherVec     = someVec.xxxx + anotherVec.yxzy;
```

You can use any combination of up to 4 ltters to create a new vector (of the same type) as long as the original vector has those components.

We can also pass vectors as arguments to different vector constructor calls

```glsl
vec2 vect        = vec2(0.4, 0.2);
vec4 result      = vec4(vect, 0.0, 0.0);
vec4 otherResult = vec4(result.xyz, 1.0);
```

### Ins and outs

Each shader can specify inputs and outpus using `in` and `out` keywords respectively. Wehenever an output variable matches with an input variable of the next shader stage, they're passed along.

The vertex shader **should** receive some form of input otherwise it would be pretty ineffective. The vertex shader differs in its input, in that it receives its input straight from the vertex data. To define how the vertex data is organized, we specify the input variables with location metadata so we can configure the vertex attributes on the CPU.

> We've seen this in the previous chapter as `layout (location = 0)`

> It is also possible to omit the `layout (location = 0)` specifier and query for the attribute locations in your OpenGL code via `glGetAttribLocation`. But setting the layout make it easier to understand and saves your (and OpenGL) some work.

The fragment shader requires a `vec4` color output variable, since the fragment shaders needs to generate a final output color. If you fail to specify an output color in your fragment shader, the color buffer output for those fragments will be undefined.

So if we want to send data from one shader to the other, we'd have to declare an output in the sending shader and a similar input in the receiving shader. When the types and the names are equal on both sides OpenGL will link those variables together and then it is possible to send data between shaders (this is done when linking a program object).

**Vertex shader**:

```glsl
#version 330 core
layout (location = 0) in vec3 aPos; // the position variable has attribute position 0

out vec4 vertexColor; // specify a color output to the fragment shader

void main()
{
    gl_Position = vec4(aPos, 1.0); // see how we directly give a vec3 to vec4's constructor
    vertexColor = vec4(0.5, 0.0, 0.0, 1.0); // set the output variable to a dark-red color
}
```

**Fragment shader**:

```glsl
#version 330 core
out vec4 FragColor;

in vec4 vertexColor; // the input variable from the vertex shader (same name and same type)

void main()
{
    FragColor = vertexColor;
}
```

### Uniforms

**`Uniforms`** are another way to pass data from out application on the CPU to the shaders on the GPU. Uniforms are **global**, meaning that uniform variable is unique per shader program object, and can be accessed from any shader at any stage in the shader program. Whatever you set the uniform value to, uniforms will keep their values until they're either reset or updated.

To declare a uniform in GLSL, we simply add the `uniform` keyword to a shader with a type and a name.

```glsl
#version 330 core
out vec4 FragColor;

uniform vec4 uColor; // we set this variable in the OpenGL code.

void main()
{
    FragColor = uColor;
}
```

---

**Warning**: If you declare a uniform that isn't used anywhere in your GLSL code, the compiler will silently remove the variable from the compiled version which is the cause for several frustating errors.

---

To set the value of a uniform, we first need to find the index/location of the uniform attribute in our shader. Once we have the index/location of the uniform, we can update its values.

```cpp
// just messing with color throughout time
float timeValue{ static_cast<float>(glfwGetTime()) };
float greenValue{ std::sin(timeValue) / 2.0f + 0.5f };

// set the uniform value
GLint vertexColorLocation{ glGetUniformLocation(m_shaderProgram, "uColor") }; // this name must match the uniform name
glUseProgram(shaderProgram);
glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);
```

We query for the location of the `uColor` uniform using `glGetUniformLocation`. We suppy the shader program and the name of the uniform to the function. If `glGetUniformLocation` returns -1, it could not find the location. **Note that finding the uniform location does not require you to use the shader program first but updating a uniform does require you to first use the program**.

We set the uniform value using the `glUniform4f` function. It is one of many functions that can be used to set the value of a uniform (GLSL does not support overloading).

> `glUniform` variations: `glUniform<N><T>[v]`
>
> Where:
>
> - <> = always exist
> - [] = optionally exist
> - \<N> = number of values: 1, 2, 3, 4.
> - \<T> = type: `f`, `i`, `ui`, `d`.
> - \[v] = the function receives an array instead of single value for each argument
>
> Example:
>
> - `glUniform4f`: expects 4 float values to be passed to the function.
> - `glUniform3iv`: expects 3 integer values to be passed as an array to the function

### More attributes

We saw in the previous chapter how we can fill a VBO, configure vertex attribute pointers and store it all in a VAO. This time, we also want to add color data to the vertex data. We're going to add color data as 3 `floats` to the `vertices` array. We assign a red, green, and blue color to each of the corners of our triangle.

```cpp
float vertices[] = {
    // positions         // colors
     0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // bottom right
    -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // bottom left
     0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // top
};
```

We need to adjust the vertex shader to also receive our color value as a vertex attribute input.

**Vertex shader**:

```glsl
#version 330 core
layout (location = 0) in vec3 aPos;   // position attribute at position 0
layout (location = 1) in vec3 aColor; // color attribute at position 1

out vec3 ourColor; // output a color to the fragment shader

void main()
{
    gl_Position = vec4(aPos, 1.0);
    ourColor = aColor; // set ourColor to input color
}
```

**Fragment shader**:

```glsl
#version 330 core
out vec4 FragColor;
in vec3 ourColor;

void main()
{
    FragColor = vec4(ourColor, 1.0);
}
```

Because we added new attribute, we need to update the vertex attribute pointers as well.

```text
          ┌───────────────────────────────────┬───────────────────────────────────┬───────────────────────────────────┐
          │ VERTEX 1                          │ VERTEX 2                          │ VERTEX 3                          │
          ├─────┬─────┬─────┲━━━━━┳━━━━━┳━━━━━╅─────┬─────┬─────┲━━━━━┳━━━━━┳━━━━━╅─────┬─────┬─────┲━━━━━┳━━━━━┳━━━━━┪
          │  X  │  Y  │  Z  ┃  R  ┃  G  ┃  B  ┃  X  │  Y  │  Z  ┃  R  ┃  G  ┃  B  ┃  X  │  Y  │  Z  ┃  R  ┃  G  ┃  B  ┃
          └─────┴─────┴─────┺━━━━━┻━━━━━┻━━━━━┹─────┴─────┴─────┺━━━━━┻━━━━━┻━━━━━┹─────┴─────┴─────┺━━━━━┻━━━━━┻━━━━━┛
    byte: 0     4     8     12    16    20    24    28    32    36    40    44    48    52    56    60    64    68    72
          ╵                 ╵                 ╵                 ╵
POSITION: ├──────────╴stride: 24╶────────────▶╵                 ╵
          │                 ╷                                   ╵
          ▼                 ╷                                   ╵
      offset: 0             ╷                                   ╵
                            ╷                                   ╵
                     COLOR: ┌──────────╴stride: 24╶────────────▶╵
                            │
                            ▼
                        offset: 12
```

Knowing the memory layout, we can update the vertex format accordingly.

```cpp
// position attrib
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
glEnableVertexAttribArray(0);

// color attrib
glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3* sizeof(float)));
glEnableVertexAttribArray(1);
```

> The image may not be exactly what you would expect, since we only supplied 3 colros, not the huge color palette.
> This is all the result of something called **fragment interpolation** in the fragment shader.

## Our own shader class

Move the code related to shader management into its own file:

- [shader.hpp](./code/shader.hpp)
- [app_with_shader_class.hpp](./code/app_with_shader_class.hpp)
