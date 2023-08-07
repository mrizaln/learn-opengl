# Transformations

## Theory

> Inserts high school linear algebra lesson (up to `3x3` matrix anyway)

The OpenGL itself works with 3D, but we usually work with `4x4` transformations matrices for several reasons and one of them is that most of the vectors are of size `4`.

```math
\vec{v}

=

\begin{pmatrix}
x \\
y \\
z \\
w \\
\end{pmatrix}
```

### Homogeneous coordinates

The $w$ component of a vector is also known as a homogeneous coordinate. To get the 3D vector from a homogenous vector we divide the $x$, $y$, and $z$ coordinate by its $w$ coordinate. We usually do not notice this since the $w$ component is $1.0$ most of the time. Using homogenous coordinates has several advantages: it allows us to do matrix translations on 3D vectors (without a $w$ component we can't translate vectors using 3D matrix), and we can use the $w$ value to create 3D perspective.

> Whenever the homogeneous coordinate is equal to $0$, the vector is specifically known as a **direction vector** since a vector with a $w$ coordinate of $0$ can't be translated.

### Identity matrix

The most simple transformation matrix that we can think of is the identity matrix. This matrix is usually used as a starting point for generating other transformation matrices.

```math
\begin{pmatrix}
{\color{red}1} & {\color{red}0} & {\color{red}0} & {\color{red}0} \\
{\color{green}0} & {\color{green}1} & {\color{green}0} & {\color{green}0} \\
{\color{blue}0} & {\color{blue}0} & {\color{blue}1} & {\color{blue}0} \\
{\color{magenta}0} & {\color{magenta}0} & {\color{magenta}0} & {\color{magenta}1} \\
\end{pmatrix}

\cdot

\begin{pmatrix}
1 \\
2 \\
3 \\
4 \\
\end{pmatrix}

=

\begin{pmatrix}
{\color{red}1} \cdot 1 \\
{\color{green}1} \cdot 2 \\
{\color{blue}1} \cdot 3 \\
{\color{magenta}1} \cdot 4 \\
\end{pmatrix}

=

\begin{pmatrix}
1 \\
2 \\
3 \\
4 \\
\end{pmatrix}
```

> Yes, it does nothing

### Scaling

Three dimensional scaling using 4D matrix is defined as follows

```math
\begin{pmatrix}
{\color{red}S_x} & {\color{red}0} & {\color{red}0} & {\color{red}0} \\
{\color{green}0} & {\color{green}S_y} & {\color{green}0} & {\color{green}0} \\
{\color{blue}0} & {\color{blue}0} & {\color{blue}S_z} & {\color{blue}0} \\
{\color{magenta}0} & {\color{magenta}0} & {\color{magenta}0} & {\color{magenta}1} \\
\end{pmatrix}

\cdot

\begin{pmatrix}
x \\
y \\
z \\
1 \\
\end{pmatrix}

=

\begin{pmatrix}
{\color{red}S_x} \cdot x \\
{\color{green}S_y} \cdot y \\
{\color{blue}S_z} \cdot z \\
1 \\
\end{pmatrix}
```

### Translation

The magic of using 4D matrix to transform a 3D vector is instead of using an addition with other vector to translate a vector, we can just multiply with a 4D matrix with the shape as follows

```math
\begin{pmatrix}
{\color{red}1} & {\color{red}0} & {\color{red}0} & {\color{red}T_x} \\
{\color{green}0} & {\color{green}1} & {\color{green}0} & {\color{green}T_y} \\
{\color{blue}0} & {\color{blue}0} & {\color{blue}1} & {\color{blue}T_z} \\
{\color{magenta}0} & {\color{magenta}0} & {\color{magenta}0} & {\color{magenta}1} \\
\end{pmatrix}

\cdot

\begin{pmatrix}
x \\
y \\
z \\
1 \\
\end{pmatrix}

=

\begin{pmatrix}
x + {\color{red}T_x} \\
y + {\color{green}T_y} \\
z + {\color{blue}T_z} \\
1 \\
\end{pmatrix}
```

This works because all of the translation values are multiplied by the vector's w column and added to the vector's original values.

### Rotation

Rotations in 3D are specified with an angle and a rotation axis. The angle specified will rotate the object along the rotation axis given.

- Rotation around the X-axis:

  ```math
  \begin{pmatrix}
  {\color{red}1    } & {\color{red}0           } &  {\color{red}0           } & {\color{red}0  } \\
  {\color{green}0  } & {\color{green}cos \theta} & -{\color{green}sin \theta} & {\color{green}0} \\
  {\color{blue}0   } & {\color{blue}sin \theta } &  {\color{blue}cos \theta } & {\color{blue}0} \\
  {\color{magenta}0} & {\color{magenta}0       } &  {\color{magenta}0       } & {\color{magenta}1} \\
  \end{pmatrix}

  \cdot

  \begin{pmatrix}
  x \\
  y \\
  z \\
  1 \\
  \end{pmatrix}

  =

  \begin{pmatrix}
  x \\
  {\color{green}cos \theta} \cdot y - {\color{green}sin \theta} \cdot z \\
  {\color{blue}sin \theta } \cdot y + {\color{blue}cos \theta } \cdot z \\
  1 \\
  \end{pmatrix}
  ```

- Rotation around the Y-axis:

  ```math
  \begin{pmatrix}
   {\color{red}cos \theta } & {\color{red}0    } & {\color{red}sin \theta   } & {\color{red}0  } \\
   {\color{green}0        } & {\color{green}1  } & {\color{green}0          } & {\color{green}0} \\
  -{\color{blue}sin \theta} & {\color{blue}0   } & {\color{blue}cos \theta  } & {\color{blue}0} \\
   {\color{magenta}0      } & {\color{magenta}0} & {\color{magenta}0        } & {\color{magenta}1} \\
  \end{pmatrix}

  \cdot

  \begin{pmatrix}
  x \\
  y \\
  z \\
  1 \\
  \end{pmatrix}

  =

  \begin{pmatrix}
  {\color{red}cos \theta} \cdot x - {\color{red}sin \theta} \cdot z \\
  y \\
  -{\color{blue}sin \theta } \cdot x + {\color{blue}cos \theta } \cdot z \\
  1 \\
  \end{pmatrix}
  ```

- Rotation around the Z-axis:

  ```math
  \begin{pmatrix}
  {\color{red}cos \theta  } & -{\color{red}sin \theta  } & {\color{red}0    } & {\color{red}0  } \\
  {\color{green}sin \theta} &  {\color{green}cos \theta} & {\color{green}0  } & {\color{green}0} \\
  {\color{blue}0          } &  {\color{blue}0          } & {\color{blue}1   } & {\color{blue}0} \\
  {\color{magenta}0       } &  {\color{magenta}0       } & {\color{magenta}0} & {\color{magenta}1} \\
  \end{pmatrix}

  \cdot

  \begin{pmatrix}
  x \\
  y \\
  z \\
  1 \\
  \end{pmatrix}

  =

  \begin{pmatrix}
  {\color{red}cos \theta} \cdot x - {\color{red}sin \theta} \cdot y \\
  {\color{green}sin \theta } \cdot x + {\color{green}cos \theta } \cdot y \\
  z \\
  1 \\
  \end{pmatrix}
  ```

Using the rotation matrices we can transform our position vectors around one of the three unit axes. To rate around an arbritrary 3D axis we can combine all 3 of them by first rotating around X-axis, then Y-axis, and then Z-axis. However, this quickly introduces a problem called **Gimbal lock**.

A better solution is to rotate around an arbitrary unit axis right away. The matrix is.... quite verbose (search for "Rotation matrix from axis and angle" in wikipedia). Still, this matrix does not completely prevent gimbal lock.

The concise equation is as follows:

```math
R = (cos \theta) I + (sin \theta) [\textbf{u}]_\times + (1 - cos \theta)(\textbf{u} \otimes \textbf{u})
```

> Yeah, I can't read that either.

The truly better approach is to use **quaternion** instead. Not only that it solves gimbal lock, but also more computationally friendly.

### Combining matrices

The true power from using matrices for transformation is that we can combine multiple transformations in a single matrix thanks to matrix-matrix multiplication. Note that matrix multiplication is not commutative. When multiplying matrices, the right-most matrix is first multiplied with the vector so you should read the multiplications from right to left.

It is advised to first do scaling operation, then rotations, and lastly translations when combining matrices.

> More or less like this: $M = T \cdot R \cdot S$

## In practice

OpenGL does not have any form of matrix or vector knowledge built-in, so we have to define our own math classes and functions. The even better approach is just to use a pre-made math libraries. One of them is [GLM](https://github.com/g-truc/glm).

### GLM

GLM stands for OpenGL Mathematics and is a header-only library.

Most of GLM's functionality that we need can be found in 3 headers files

```cpp
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
```

The operation using GLM is just like the in the math.

For example, a vector $(1,0,0)$ translated by $(1,1,0)$

```cpp
glm::vec4 vec{ 1.0f, 0.0f, 0.0f, 1.0f };  // don't forget the w-component needs to be 1 in order for this to work
glm::mat4 trans(1.0f);                    // identity matrix
trans = glm::translate(trans, glm::vec4{ 1.0f, 1.0f, 0.0f });
vec = trans * vec;
std::cout << vec.x << " | " << vec.y << " | " << vec.z << '\n';
```

Rotation and scaling

```cpp
glm::mat4 trans(1.0f);      // identity matrix
trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3{ 0.0, 0.0, 1.0 });  // rotate by 90 degrees around z axis
trans = glm::scale(trans, glm::vec3{ 0.5, 0.5, 0.5 });    // scale each target component by each component of this vector
```

etc.

The next question is: how do we get the transformation matrix to the shaders? Well, GLSL also mat a `mat4`! So we'll adapt the vertex shader to accept a `mat4` uniform variable and multiply the position vector by the matrix uniform.

> GLSL also has `mat2` and `mat3` types.

```glsl
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat4 tranform;

void main()
{
    gl_Position = transform * vec4(aPos, 1.0f);
    TexCoord = vec2(aTexCoord.x, aTexCoord.y);
}
```

Then we can pass the previous transformation into the shader:

```cpp
unsigned int transformLoc = glGetUniformLocation(ourShader.ID, "transform");
glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));
```

---

**Note**: GLGL use **column-major** ordering for its `mat` types unlike C and C++ that uses **row-major** ordering. However, GLM stores their matrix layout in column-major ordering, so there is no need to transpose their matrices'

---
