# Hello Triangle

## OpenGL graphics pipeline

In OpenGL everything is in 3D space, but the screen or window in a 2D array of pixels so a large part of OpenGL's work is about transforming all 3D coordinates to 2D pixels that fit on your screen. The process of transforming 3D coordinates to 2D pixel is managed by the **`graphics pipeline`** of OpenGL.

The graphics pipeline can be divided into two large parts:

- The first transforms your 3D coordinates into 2D coordinates, and
- The second part transforms the 2D coordinates into actual colored pixels.

The graphics pipeline can be divided further into several steps where each step requires the output of the previous step as its input. All of these steps are highly specialized and can be easily be executed in parallel. Each of this step is a program, and these programs are called **`shaders`**.

Some of these shaders are configurable by the developer.

> Shaders are written in OpenGL Shading Language (GLSL)

```text
                              [ The OpenGL Graphics Pipeline ]

                    ┏━━━━━━━━━━━━━━━┓       ╭────────────────╮       ┏━━━━━━━━━━━━━━━━━┓
vertex data[] ────▶ ┃ VERTEX SHADER ┃ ────▶ │ SHAPE ASSEMBLY │ ────▶ ┃ GEOMETRY SHADER ┃
                    ┗━━━━━━━━━━━━━━━┛       ╰────────────────╯       ┗━━━━━━━━━━━━━━━━━┛
                                                                              │
                                                                              │
                                                                              ▼
               ╭────────────────────╮       ┏━━━━━━━━━━━━━━━━━┓       ╭───────────────╮
               │ TESTS AND BLENDING │ ◀──── ┃ FRAGMENT SHADER ┃ ◀──── │ RASTERIZATION │
               ╰────────────────────╯       ┗━━━━━━━━━━━━━━━━━┛       ╰───────────────╯

                       ┏━┓                    ╭─╮
                       ┃ ┃ = configurable     │ │ = not configurable
                       ┗━┛                    ╰─╯
```

As input to the graphics pipeline, we pass in a list of three 3D coordinates that should form a triangle in an array here called `vertex data`. A **`vertex`** is a collection of data per 3D coordinate. This vertex's data is represented using **`vertex attributes`** that can contain any data we'd like.

> In order for OpenGL to know what to make of your collection of coordinates and color values, OpenGL requires you to hint what kind of render types you want to form with the data. Those hints are called **`primitives`** and are given to OpenGL while calling any of the drawing commands.
>
> Some of these hints are `GL_POINTS`, `GL_TRIANGLES`, and `GL_LINE_STRIP`.

<br>

**The graphics pipeline can be summarize as follow:**

1. **`Vertex shader`** receives a vertex. The main pupose of the vertex shader is to transform 3D coordinates into different 3D coordinates.

2. The output of the vertex shader is optionally passed to **`geometry shader`**. The geometry shader takes as input a collection of vertices that form a primitive and has the ability to generate other shapes by emitting new vertices to form new (or other) primitive(s).

3. The **`primitive assembly`** stage takes as input all the vertices from the vertex (or geometry) shader that form one or more primitives and assembles all the points in the primitive shape given.

4. The output of previous step is then passed on to the **`rasterization stage`** where it maps the resulting primitive(s) to the corresponding pixels on the final screen resulting in fragments. Before the fragment shader run, **`clipping`** is performed. Clipping discards all fragments that are outside your view.

5. The fragments from previous step then used by **`fragment shader`** to calculate the final color of a pixel (this is usually where all the advanced OpenGL effects occur). Usually, the fragment shader contains data about the 3D scene that it can use to calculate the final pixel colo (like lights, shadows, color of the light, etc.)

6. The last step is **`alpha test and blending`** stage. This stage checks the corresponding depth (and stencil) value of the fragment and uses those to check if the resulting fragment is in front or behind other objects and should be discarded accordingly. The stage also checks for _alpha_ values.

> In Modern OpenGL, we are **required** to define at least a vertex and fragment shader of our own.

## Normalized device coordinates

OpenGL only process 3D coordinates when they're in a specific range between `-1.0` and `1.0` on all 3 axes. All coordinates within this so called **`normalized device coordinates (NDC)`** range will end up visible on your screen (and all coordinates outside this region won't).

Unlike usual screen coordinates, the positive y-axis points in the up direction and the `(0, 0)` coordinates are at the center of the graph instead of top-left.

```text
          OpenGL             Screen coordinate
  1 ▲ ┌───────────┐        0 ▲ •───────────┐
    │ │           │          │ │           │
    │ │     •     │          │ │           │        • = origin
    │ │           │          │ │           │
 -1 ▼ └───────────┘      600 ▼ └───────────┘
      ◀──────────▶             ◀──────────▶
      -1         1             0         800
```

Your NDC will then be transformed to **`screen-space coordinates`** via the **`viewport transform`** using the data you provided with `glViewport`. The resulting screen-space coordinates are then transformed to fragments as inputs to your fragment shader.

## Vertex input

We manage the memory of the vertex data that we would like to send to the vertex shader via so called **`vertex buffer objects (VBO)`** that can store a large number of vertices in the GPU's memory. The advantage of using thos buffer objects is that we can send large batches of data all at once to the graphics card, and keep it there if there's enough memory left, without having to send data one vertex at a time.

Our vertex data:

```cpp
using Point    = std::array<float, 3>;
using Triangle = std::array<Point, 3>;

constexpr Triangle vertices{
    { { -0.5f, -0.5f, 0.0f },
      { 0.5f, -0.5f, 0.0f },
      { 0.0f, 0.5f, 0.0f } }
};

```

A vertex buffer object is an OpenGL object. Just like any object in OpenGL, this buffer has a unique ID corresponding to that buffer, so we can generate one with a buffer ID using the `glGenBuffers` function.

```cpp
GLuint vbo;
glGenBuffers(1, &vbo);
```

OpenGL has many types of buffer objects and the buffer type of a vertex buffer object is `GL_ARRAY_BUFFER`.

```cpp
glBindBuffer(GL_ARRAY_BUFFER, vbo);
```

From that point on, any buffer calls we make will be used to configure the currently bound buffer, which is `vbo`. Then we can make a call to the glBufferData function that copies the previously defined vertex data into the buffer's memory.

```cpp
glBufferData(GL_ARRAY_BUFFER, sizeof(Triangle), (void*)&vertices.front().front(), GL_STATIC_DRAW);
```

`glBufferData` is a function specifically targeted to copy user-defined data into the currently bound buffer.
The fourth parameter of it specifies how we want the graphics card to manage the given data:

- `GL_STREAM_DRAW`: the data is set only once and used by the GPU at most a few times.
- `GL_STATIC_DRAW`: the data is set only once and used many times.
- `GL_DYNAMIC_DRAW`: the data is changed a lot and used many times.

## Vertex shader

Basic vertex shader:

```glsl
#version 330 core                     // (1) OpenGL version (since OpenGL 3.3, this version matches OpenGL version)

// in <attribute>                     // (2) Input vertex attribute (if exist)

layout (location = 0) in vec3 aPos;   // (3) Set the location of the input variable

void main()
{
    gl_Position = vec4(aPos, 1.0);    // (4) Set the output of the vertex shader (predefined variable)
}
```

In order for OpenGL to use the shader, it has to dynamically compile it at run-time from its source code. The first thing we need to do is create a shader object, again, referenced by an ID.

```cpp
constexpr std::string_view vertexShaderSource = R"glsl(
    #version 330 core
    layout (location = 0) in vec3 aPos;

    void main()
    {
        gl_Position = vec4(aPos, 1.0);
    }
)glsl";

GLuint vertexShader{ glCreateShader(GL_VERTEX_SHADER) };
```

Next we attach the shader source code to the shader object and compile the shader.

```cpp
const char* vertexShaderSourceCharPtr{ gvertexShaderSource.data() };
glShaderSource(vertexShader, 1, &vertexShaderSourceCharPtr, nullptr);    // it expects an array of const char* (sigh, I don't like C...)
glCompileShader(vertexShader);
```

## Fragment shader

Basic fragment shader:

```glsl
#version 330 core

out vec4 FragColor;

void main()
{
    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
}
```

The fragment shader only requires one output variable and that is a vector of size 4 that defines the final color output that we should calculate ourselves.

The process for compiling a fragment shader is similar to the vertex shader, although this time we use the `GL_FRAGMENT_SHADER` as the shader type.

Both the shaders are now compiled and the only thing left to do is link both shader objects into a **`shader program`**.

## Shader program

A shader program object is the final linked version of multiple shaders combined. To use the recently compiled shaders we have to **link** them to a shader program object and then activate this shader program when rendering objects.

Creating shader program:

```cpp
GLuint shaderProgram{ glCreateProgram() };
```

Attach the shaders then link them to the shader:

```cpp
glAttachShader(shaderProgram, vertexShader);
glAttachShader(shaderProgram, fragmentShader);
glLinkProgram(shaderProgram);
```

The result is a program object that we can activate by calling `glUseProgram` with the newly created program object as its argument:

```cpp
glUseProgram(shaderProgram);
```

Also delete the shader objects once we've linked them into the program object; we no longer need them anymore.

```cpp
glDeleteShader(vertexShader);
glDeleteShader(fragmentShader);
```

## Linking vertex attributes

The vertex shader allows us to specify any input we want int the form of vertex attributes. Though, we still need to specify how OpenGL should interpret the vertex data before rendering.

Our vertex buffer memory layout:

```text
      ┌─────────────────┬─────────────────┬─────────────────┐
      │ VERTEX 1        │ VERTEX 2        │ VERTEX 3        │
      ├─────┬─────┬─────┼─────┬─────┬─────┼─────┬─────┬─────┤
      │  X  │  Y  │  Z  │  X  │  Y  │  Z  │  X  │  Y  │  Z  │
      └─────┴─────┴─────┴─────┴─────┴─────┴─────┴─────┴─────┘
BYTE: 0     4     8     12    16    20    24    28    32    36
      ────STRIDE:12────▶
      │
      │
      ▼
   OFFSET:0

note: [the vertex buffer is an array of triple floats (size: 4 bytes)]
```

With the knowledge above, we can tell OpenGL how it should interpret the vertex data (per vertex attribute) using `glVertexAttribPointer`:

```cpp
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
glEnableVertexAttribArray(0);
```

> Params:
>
> - **1st**: which vertex attribute we want to configure -> (0; see the layout on the `vertexShaderSource`)
> - **2nd**: the size of the vertex attribute -> (3; 3 vec3 of float)
> - **3rd**: specify the type of the data -> (float)
> - **4th**: specify whether we want the data to be normalized -> (GL_FALSE)
>   - If we're inputting integer data types and we've set this to `GL_TRUE`, the integer data is normalized to 0 (-1 for signed data) and 1 when converted to float
> - **5th**: stride -> (12; just as above: 3 \* sizeof(float) = 12)
> - **6th**: offset -> (0)

---

**`Note`**:

Each vertex attribute takes its data from memory managed by a VBO and which VBO it takes its data from is determined by the VBO currently bound to `GL_ARRAY_BUFFER` when calling `glVertexAttribPointer`.

---

## Vertex array object

A vertex array object (VAO) can be bound like a vertex buffer object and any subsequent vertex attribute calls from that point on will be stored inside the VAO. This has the advantage that when configuring vertex attribute pointers, you only have to make those calls once and whenever we want to draw the object, we can just bind the corresponding VAO.

> Core OpenGL requires that we use a VAO so it knows what to do with our vertex inputs. If we fail to bind a VAO, OpenGL will most likely refuse to draw anything.

A vertex array object stores the following:

- Calls to `glEnableVertexAttribArray` or `glDisableVertexAttribArray`.
- Vertex attribute configurations via `glVertexAttribPointer`.
- Vertex buffer objects associated with vertex attributes by calls to `glVertexAttribPointer`.

The process to generate a VAO looks similar to that of VBO:

```cpp
GLuint vao{};
glGenVertexArrays(1, &vao);
```

To use a VAO all you have to do is bind the VAO using `glBindVertexArray`. From that point on, we should bind/configure the corresponding VBO(s) and attribute pointer(s) and then unbind the VAO for later use. As soon as we want to draw an object, we simply bind the VAO with the preferred settings before drawing the object.

```cpp
// configure vertex
glBindVertexArray(vao);

glBindBuffer(GL_ARRAY_BUFFER, vbo);
glBufferData(GL_ARRAY_BUFFER, sizeof(Triangle), (void*)&vertices.front().front(), GL_STATIC_DRAW);

glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
glEnableVertexAttribArray(0);

// ...

// draw
glUseProgram(shaderProgram);
glBindVertexArray(vao);

// someOpenGLFunctionThatDrawsOurTriangle();
```

## The triangle we've all been waiting for

To draw our objects of choice, OpenGL provides us with the `glDrawArrays` function that draws primitives using the currently active shader, the previously defined vertex attribute configuration, and with the VBO's vertex data (indirectly bound via the VAO).

```cpp
glUseProgram(app::shaderProgram);
glBindVertexArray(app::triangleVao);
glDrawArrays(GL_TRIANGLES, 0, g_vertices.size());
```

## Element buffer object

An element buffer object (EBO) is a buffer, just like a VBO, that stores indices that OpenGL uses to decide what vertices to draw. This is also so called **indexed drawing** (useful when we want to draw vertices without duplicating the data).

For example, we want to draw a rectangle. To create one, we need two triangles, thus 6 vertices. Using EBO, our data then can be divided into vertices and indices:

```cpp
float vertices[] = {
     0.5f,  0.5f, 0.0f,  // top right
     0.5f, -0.5f, 0.0f,  // bottom right
    -0.5f, -0.5f, 0.0f,  // bottom left
    -0.5f,  0.5f, 0.0f   // top left
};
unsigned int indices[] = {  // note that we start from 0!
    0, 1, 3,   // first triangle
    1, 2, 3    // second triangle
};
```

The construction of the EBO is similar to VBO.

```cpp
GLuint ebo;
glGenBuffers(1, &ebo);
glBindBuffer(GL_ARRAY_BUFFER, ebo);
glBufferData(GL_ARRAY_BUFFER, sizeof(Triangle), (void*)&vertices.front().front(), GL_STATIC_DRAW);
```

The draw call using EBO is different

```cpp
glBindVertexArray(vao);
glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
```
