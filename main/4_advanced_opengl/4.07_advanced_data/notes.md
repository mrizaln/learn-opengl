# Advanced Data

Throughout the chapters, we've been extensively using buffers in OpenGL to store data on the GPU. A buffer in OpenGL is an object that manages a certain piece of GPU memory and nothing more. We give it meaning when binding it to a specific **buffer target**. OpenGL internally stores a reference to the buffer per target and, based on the target, processes the buffer differently.

So far, we've been filling the buffer;s memory by calling `glBufferData`, which allocates a piece of GPU memory and adds data into this memory. If we were to pass `NULL` as its data argument, the function would only allocate memory and not fill it. This is useful if we first want to reserve specific amount of memory and later come back to this buffer.

Instead of filling the entire buffer with one function call, we can also fill specific regions of the buffer by calling `glBufferSubData`. This allows us to insert/update only cdrtain parts of the buffer's memory.

> Note that the buffer should have enough allocated memory so a call to `glBufferData` is necessary before calling `glBufferSubData`

```cpp
// function prototype
void glBufferSubData(GLenum target, GLintptr offset, GLsizeptr size, const GLvoid* data);

// function call example
glBufferSubData(GL_ARRAY_BUFFER, 24, sizeof(data), &data);
```

Another method for getting data into a buffer is to ask for a pointer to the buffer's memory and directly copy the data in memory yourself. By calling `glMapBuffer` OpenGL returns a pointer to the currently bound buffer's memory for us to operate on.

```cpp
float data[]{
    0.5f, 1.0f, -0.35f,
    // ...
};

glBindBuffer(GL_ARRAY_BUFFER, buffer);

// get pointer
void* ptr{ glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY) };

// now copy data into memory
memcpy(ptr, data, sizeof(data));

// tell OpenGL, we're done with the pointer
glUnmapBuffer(GL_ARRAY_BUFFER);
```

> `glUnmapBuffer` returns `GL_TRUE` if OpenGL was able to map your data successfully to the buffer.

## Batching vertex attributes

Using `glVertexAttribPointer`, we were able to specify the attribute layout of the vertex array buffer's content. Within the vertex array buffer we interleaved the attributes for each vertex. Now that we know a bit more about buffers, we can take a different approach.

What we could also do is batch all the vector data into large chunks per attribute type instead of interleaving them. Instead of `ABCABCABCABC` layout, we take a `AAAABBBBCCCC` layout.

We can implement this batching using `glBufferSubData` for example:

```cpp
float positions[]{ ... };
float normals[]{ ... };
float tex[]{ ... };

// fill buffer
glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(positions), &positions);
glBufferSubData(GL_ARRAY_BUFFER, sizeof(positions), sizeof(normals), &normals);
glBufferSubData(GL_ARRAY_BUFFER, sizeof(positions) + sizeof(normals), sizeof(tex), &tex);
```

We also have to update the vertex attribute pointers to reflect these changes

```cpp
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(sizeof(positions)));
glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)(sizeof(positions) + sizeof(normals)));
```

Note that the `stride` parameter is equal to the size of the vertex attribute and the offset is equal to the size of the said attribute (all vertex).

## Copying buffers

Once your buffers are filled with data, you may want to share that data with other buffers or perhaps copy the buffer's content into another buffer. The function `glCopyBufferSubData` allows us to copy the data from one buffer to the other.

```cpp
// function prototype
void glCopyBufferSubData(GLenum readtarget, GLenum writetarget, GLintptr readoffset, GLintptr writeoffset, GLsizeptr size);
```

The `readtarget` and `writetarget` parameters expect to give the buffer targets that we want to copy from and to. We could for example copy from a `VERTEX_ARRAY_BUFFER` buffer to a `VERTEX_ELEMENT_ARRAY_BUFFER` buffer by specifying those buffer targets as the read and write targets respectively. The buffers currently bound to those buffer targets will then be affected.

But what if we wanted to read and write data into two different buffers that are both vertex array buffers? We can't bind two buffers at the same time to the same buffer target. For this reason (and this reason alone), OpenGL gives us two more buffer targets called `GL_COPY_READ_BUFFER`, and `GL_COPY_WRITE_BUFFER`. We then bind the buffers of our choice to these new buffer targets and set those targets as the `readtarget` and `writetarget` argument.

```cpp
glBindBuffer(GL_COPY_READ_BUFFER, vbo1);
glBindBuffer(GL_COPY_WRITE_BUFFER, vbo2);
glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, 8 * sizeof(float));
```

We could've also done this by only binding the `writetarget` buffer to one of the new buffer target types

```cpp
glBindBuffer(GL_ARRAY_BUFFER, vbo1);
glBindBuffer(GL_COPY_WRITE_BUFFER, vbo2);
glCopyBufferSubData(GL_ARRAY_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, 8 * sizeof(float));
```
