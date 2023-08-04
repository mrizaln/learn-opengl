# Hello Window

We uses GLFW to create the window and uses OpenGL version 3.3. To load the the OpenGL functions, we use [glad](https://github.com/Dav1dde/glad).

## Preparation

### Initialize GLFW and create window

Initialize GLFW using

```cpp
glfwInit()
```

Then set the OpenGL version and profile using

```cpp
glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
```

After that, create the window

```cpp
GLFWwindow* window{ glfwCreateWindow(
    <window_width>,
    <window_height>,
    <window_name>,
    nullptr,
    nullptr
) };

```

Make sure to set the context of the window to the current thread.

```cpp
glfwMakeContextCurrent(window);
```

### Load glad

Load glad using

```cpp
gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)
```

### Set viewport

Set the rendering window size

```cpp
// origin is at the bottom left corner of the window
glViewport(<location_x>, <location_y>, <window_width>, <window_height>);
// example.: glViewport(0, 0, 800, 600);
```

Behind the scenes, OpenGL uses the data specified via `glViewport` to transform the 2D coordinates it processed to coordinates on your screen. For example, a processed point of location `(-0.5, 0.5)` would be mapped to `(200, 450)` in screen coordinates of resolution `800x600`.

Note that processed coordinates in OpenGL are between `-1 and 1`, so we effectively map from the range `[-1, 1]` to `[0, 800]` and `[0, 600]`.

```text
          OpenGL                              Screen coordinate
  1 ▲ ┌───────────┐                       600 ▲ ┌───────────┐
    │ │  p        │  p = (-0.5, 0.5)          │ │  p        │  p = (200, 450)
    │ │           │                           │ │           │
    │ │           │                           │ │           │
 -1 ▼ └───────────┘                         0 ▼ └───────────┘
      ◀──────────▶                              ◀──────────▶
      -1         1                              0         800
```

The moment, a user resizes the window, the viewport should be adjusted as well. We can register to the resize event using callback:

```cpp
glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
// framebuffer_size_callback signature: void(GLFWwindow* window, int width, int height)
```

## Render loop

We don't want the application to draw a single image and then immediately quit and close the window. We want it to keep drawing images and handling user input until the program explicitly told to stop. This is why we need a loop, _render loop_.

```cpp
while (!glfwWindowShouldClose(window)) {
    glfwSwapBuffers(window);    // swap the buffer; by default GLFW is double bufferred
    glfwPollEvents();           // checks if any events are triggered
}
```

## Terminating

As soon as we exit the render loop, we would like to properly clean/delete all of GLFW's resources that were allocated.

```cpp
glfwTerminate();
```

## Input

GLFW gives several method of managing input: [GLFW input guide](https://www.glfw.org/docs/3.3/input_guide.html). But, for continuous input of key events, we can query it directly using `glfwGetKey` function.

For example, this is a function that process input every frame.

```cpp
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}
```

```cpp
while (!glfwWindowShouldClose(window)) {
    processInput(window);       // process input every frame

    glfwSwapBuffers(window);    // swap the buffer; by default GLFW is double bufferred
    glfwPollEvents();           // checks if any events are triggered
}
```

## Rendering

We want to put all the rendering commands in the render loop, since we want to execute all the rendering commands each frame of the loop.

Something like this:

```cpp

while (!glfwWindowShouldClose(window)) {
    processInput(window);       // process input every frame

    // rendering commands here
    // example:
    glClearColor(0.1f, 0.1f, 0.11f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glfwSwapBuffers(window);    // swap the buffer; by default GLFW is double bufferred
    glfwPollEvents();           // checks if any events are triggered
}
```
