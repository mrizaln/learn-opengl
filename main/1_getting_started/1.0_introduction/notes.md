# OpenGL

OpenGL is mainly considered an API that provides us with a large set of functions that we can use to manipulate graphics and images. However, OpenGL by itself is not an API, but merely a specification, developed and maintained by the Khronos Group.

## State machine

OpenGL is by itself a large state machine: a collection of variables that define how OpenGL should currently operate. The state of OpenGL is commonly referred to as the OpenGL **`context`**. When using OpenGL, we often change its state by setting some options, manipulating some buffers and then render using the current context.

## Objects

An object in OpenGL is a collection of options that represents a subset of OpenGL's state. For example, we could have an object that represents the settings of the drawing window; we could then set its size, how many colors it supports and so on.

One could visualize an object as a C-like struct

```cpp
struct object_name {
    float  option1;
    int    option2;
    char[] name;
};
```

Whenever we want to use objects it generally looks something like this:

```cpp
// create object
unsigned int objectId{};
glGenObject(1, &objectId);

// bind/assign object to context
glBindObject(GL_WINDOW_TARGET, objectId);

// set options of object currently bound to GL_WINDOW_TARGET
glSetObjectOption(GL_WINDOW_TARGET, GL_OPTION_WINDOW_WIDTH, 800);
glSetObjectOption(GL_WINDOW_TARGET, GL_OPTION_WINDOW_HEIGHT, 600);

// set context target back to default
glBindObject(GL_WINDOW_TARGET, 0);
```
