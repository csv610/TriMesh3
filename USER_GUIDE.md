# trimesh2 User Guide

`trimesh2` provides a robust and efficient set of tools for processing and visualizing 3D triangular meshes. This guide covers common use cases and library features.

## Core Concepts

### TriMesh Class

The `TriMesh` class is the central data structure for representing 3D meshes. It stores vertices, faces, normals, colors, and other optional per-vertex properties.

```cpp
#include "TriMesh.h"

trimesh::TriMesh *mesh = trimesh::TriMesh::read("input.ply");

// Property calculation
mesh->need_normals();
mesh->need_connectivity();
mesh->need_bbox();
mesh->need_curvatures();

// Accessing mesh data
std::vector<trimesh::point> &vertices = mesh->vertices;
std::vector<trimesh::TriMesh::Face> &faces = mesh->faces;
```

### Properties and Lazy Computation

`trimesh2` uses a lazy-computation model for mesh properties. You should explicitly request the properties you need using the `need_*()` family of functions.

- `need_normals()`: Computes vertex and face normals.
- `need_connectivity()`: Builds connectivity information (faces-per-vertex, etc.).
- `need_tstrips()`: Generates triangle strips for faster rendering.
- `need_curvatures()`: Computes principal curvatures and directions.

## Algorithms

### KD-Tree Search

The `KDtree` class provides fast nearest-neighbor search for points.

```cpp
#include "KDtree.h"

std::vector<trimesh::point> points = ...;
trimesh::KDtree tree(points);

trimesh::point query_point(1.0, 2.0, 3.0);
const float *closest = tree.closest_to(query_point);
```

### ICP Alignment

The library provides tools for aligning two meshes using the Iterative Closest Point (ICP) algorithm.

```cpp
#include "ICP.h"

trimesh::TriMesh *mesh1 = ...;
trimesh::TriMesh *mesh2 = ...;

// Alignment of mesh2 to mesh1
trimesh::xform xf;
float err = trimesh::ICP(mesh1, mesh2, xf);
```

## GUI Development with `gluit`

The `gluit` library simplifies building OpenGL-based applications for mesh visualization. It provides wrappers for GLUT and additional GUI controls.

### Using `GLCamera`

The `GLCamera` class handles 3D viewport manipulation, such as rotations, panning, and zooming.

```cpp
#include "GLCamera.h"

trimesh::GLCamera camera;
camera.setupGL(origin, radius);
```

## Command-Line Utilities

A suite of pre-built tools is available in the `bin/` directory after building:

- `mesh_view`: A viewer for 3D meshes.
- `mesh_align`: Align two or more meshes.
- `mesh_filter`: Apply various filters (smoothing, noise removal, etc.).
- `mesh_info`: Display detailed information about a mesh.
- `mesh_make`: Create basic primitives like spheres, cubes, and tori.

## Troubleshooting

- **Performance**: For large meshes, ensure triangle strips are used for rendering (`need_tstrips()`).
- **Precision**: The library defaults to single-precision floating point. For double precision, you can use `Vec<3, double>` and other templates.
