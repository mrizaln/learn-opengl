# Advanced GLSL

## GLSL's built-in variables

There are a few extra variables define by GLSL prefixed with `gl_` that gives us an extra means to gather and/or write data.

> We've already see two of them: `gl_Position` and `gl_FragCoord`

We'll discuss a few interesting built-in input and output variables that are built-in in GLSL.

> All of the built-in GLSL variables can be found [here](https://www.khronos.org/opengl/wiki/Built-in_Variable_%28GLSL%29)

## Vertex shader variables

We've already seen `gl_Position` which is the clip-space output position vector of the vertex shader. Setting `gl_Position` in the vertex shader is a strict requirement if you want to render anything on the screen.

### `gl_PointSize`

One of the render primitives we're able to choose from is `GL_POINTS` in which case each single vertex is a primitive and rendered as a point. It is possible to set the size of the points being rendered via OpenGL's `glPointSize` function, but we can also influence this value in the vertex shader.

One output variable defined by GLSL is called `gl_PointSize` that is a `float` variable where you can set the point's width and height in pixels.
