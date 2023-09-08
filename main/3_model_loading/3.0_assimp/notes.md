# Assimp
Complex models are usually built in a 3D modelling tool. The model itself is then saved into a file. There are various file type with different formatting. These technical details are hidden from the modeler (because the modeler does not need to actually care how the model is saved into the file). We, as graphics programmer though, have to care about these technical details.

All in all, there are many different file formats where a common general structure between them usually does not exist. We'd have to write an importer ourselves for each file formats. Luckily, there just happens to be a library for this.


## A model loading library

A very popular model importing library out there is called **Assimp** that stands for _Open Asset Import Library_. Assimp is able to import dozens of different file formats by loading the model's data into Assimp's generalized data structures.

When importing a model via Assimp it loads the entire model into a `Scene` object that contains all the data of the imported model/scene. Assimp then has a collection of nodes where each node contains indices to data stored in the scene object where each node can have any number of children.

- All the data of the scene/model is contained in the `Scene` object like all materials and the meshes.
- The **Root node** of the scene may contain children nodes and could have a set of indices that point to mesh data in the scene object's `mMeshes` array. The scene's `mMeshes` array contains the actual `Mesh` objects, the values in the `mMeshes` array of a node are only indices for the scene's meshes array.
- A `Mesh` object itself contains all the relevant data required for rendering (vertex positions, normal vectors, texture coordinates, faces, and the material of the object).
- A mesh contains several faces. A `Face` represents a render primitive of the object (triangles, squares, points). A face contains the indices of the vertices that form a primitive. Because the vertices and the indices are separated, this makes it easy for us to render via an index buffer.
- Finally a mesh also links to a `Material` object that hosts several functions to retrieve the material properties of an object (colors and/or texture maps).

What we want to do is: first load an object into a `Scene` object, recursively retrieve the corresponding `Mesh` objects from each of the nodes , and process each `Mesh` object to retrieve the vertex data, indices, and its material properties. The result is then a collection of mesh data that we want to contain in a single `Model` object.
