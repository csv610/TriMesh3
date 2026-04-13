# trimesh2

`trimesh2` is a C++ library for processing and viewing 3D triangular meshes. Originally developed by **Szymon Rusinkiewicz** at Princeton University, it includes a variety of algorithms for mesh processing, such as curvature computation, ICP (Iterative Closest Point), and mesh filtering, as well as a library for building simple OpenGL-based GUIs.

## Modernization & Apple Silicon Port

This repository represents a modernization effort focused on porting the original codebase to the **Apple Silicon (M1/M2/M3)** architecture and ensuring production-level stability on modern macOS systems.

### Key Contributions:
- **Architecture Porting**: Resolved architecture-specific compiler flags and compatibility issues for `arm64` (Apple Silicon).
- **Build System Migration**: Replaced the original Makefile-based system with a modern **CMake** build infrastructure, including proper installation targets.
- **Compiler Warning Resolution**: Addressed pervasive implicit conversion and deprecation warnings to achieve a clean build on modern Clang/LLVM.
- **Robust Testing**: Implemented an extensive automated unit test suite using `ctest`, achieving high code coverage across core library components.
- **Code Standards**: Introduced `.clang-format` and updated documentation for professional maintainability.

## Features

- **Mesh Processing**: Normal computation, curvature estimation, subdivision, simplification, and filtering.
- **Algorithms**: Efficient KD-tree, ICP-based alignment, and connectivity analysis.
- **I/O Support**: PLY, OBJ, OFF, and other common 3D formats.
- **Utilities**: A suite of command-line tools for mesh manipulation (e.g., `mesh_view`, `mesh_align`, `mesh_filter`).
- **GUI Toolkit**: `gluit` library for quickly building interactive 3D visualizations.

## Building the Project

### Prerequisites

- CMake (>= 3.10)
- A C++11 compatible compiler (Clang, GCC, MSVC)
- OpenGL and GLUT (for `gluit` and `mesh_view`)

### Build Instructions

```bash
mkdir build
cd build
cmake ..
make
```

### Running Tests

```bash
cd build
ctest
```

## Quick Start Example

```cpp
#include "TriMesh.h"
#include <iostream>

int main() {
    trimesh::TriMesh *mesh = trimesh::TriMesh::read("model.obj");
    if (!mesh) {
        std::cerr << "Error reading mesh" << std::endl;
        return 1;
    }

    mesh->need_normals();
    std::cout << "Mesh has " << mesh->vertices.size() << " vertices." << std::endl;

    delete mesh;
    return 0;
}
```

## Installation

```bash
cd build
sudo make install
```

## License

This project is licensed under the terms found in the `COPYING` file.
