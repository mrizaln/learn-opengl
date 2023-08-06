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
