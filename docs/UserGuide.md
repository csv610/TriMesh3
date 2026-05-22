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

#### Data Members

The TriMesh class stores the following data, organized by category:

**Basic Geometry**
- `vertices`: Vector of 3D points (x, y, z coordinates)
- `faces`: Vector of faces, where each Face is a `Vec<3, int>` containing three vertex indices
- `tstrips`: Triangle strips for efficient rendering
- `grid`: Grid representation for range images (with `grid_width` and `grid_height`)

**Per-Vertex Properties**
- `colors`: Per-vertex colors (RGB)
- `confidences`: Confidence values per vertex
- `flags`: Per-vertex flags (unsigned integers)
- `normals`: Vertex normals
- `pdir1`, `pdir2`: Principal curvature directions
- `curv1`, `curv2`: Principal curvature values
- `dcurv`: Curvature derivatives (4-component tensor)
- `cornerareas`: Corner areas for mesh processing
- `pointareas`: Point areas for mesh processing

**Bounding Structures**
- `bbox`: Axis-aligned bounding box (`BBox`)
- `bsphere`: Bounding sphere with center and radius

**Connectivity Structures**
- `neighbors`: For each vertex, all neighboring vertices
- `adjacentfaces`: For each vertex, all faces containing that vertex
- `across_edge`: For each face, the three faces sharing its edges

**Internal State**
- `flag_curr`: Current flag value for iteration

## Mesh Formats (Input and Output)

`TriMesh::read()` and `TriMesh::write()` automatically detect and convert between formats based on file extension.

### Supported Input Formats

| Format | Extensions | Description |
|--------|------------|-------------|
| PLY   | `.ply`     | Polygon file format (ASCII or binary, triangle mesh or range grid) |
| OFF   | `.off`     | Object File Format |
| OBJ   | `.obj`     | Wavefront OBJ |
| RAY   | `.ray`     | Ray-traced scene format |
| SM    | `.sm`      | Simple mesh format |
| 3DS   | `.3ds`     | 3D Studio mesh |
| VVD   | `.vvd`     | Vivid volumetric format |
| STL   | `.stl`     | Stereolithography (ASCII or binary) |
| PTS   | `.pts`     | Point cloud format |

### Supported Output Formats

| Format | Extensions | Description |
|--------|------------|-------------|
| PLY   | `.ply`     | Polygon file format |
| OFF   | `.off`     | Object File Format |
| OBJ   | `.obj`     | Wavefront OBJ (with optional normals) |
| RAY   | `.ray`     | Ray-traced scene format |
| SM    | `.sm`      | Simple mesh format |
| STL   | `.stl`     | Stereolithography |
| PTS   | `.pts`     | Point cloud format |
| C++   | `.cc`      | C++ source code (mesh as static arrays) |
| DAE   | `.dae`     | Collada digital asset exchange |

### Usage Examples

```cpp
trimesh::TriMesh *mesh = trimesh::TriMesh::read("model.obj");

// Convert from OBJ to PLY
mesh->write("output.ply");
```

## Properties and Lazy Computation

`trimesh2` uses a lazy-computation model for mesh properties. Properties are computed on demand via `need_*()` functions and cached until explicitly cleared.

### Property Computation Functions

| Function | Description |
|----------|-------------|
| `need_faces()` | Generates faces from tstrips or grid if needed |
| `need_tstrips(TstripRep rep)` | Generates triangle strips for efficient rendering |
| `need_normals(bool simple_area_weighted)` | Computes vertex normals; optionally use simple area-weighted method |
| `need_curvatures()` | Computes principal curvatures and directions |
| `need_dcurv()` | Computes curvature derivatives |
| `need_pointareas()` | Computes point and corner areas |
| `need_bbox()` | Computes axis-aligned bounding box |
| `need_bsphere()` | Computes bounding sphere |
| `need_neighbors()` | Builds vertex-to-vertex adjacency |
| `need_adjacentfaces()` | Builds vertex-to-face adjacency |
| `need_across_edge()` | Builds face-to-face adjacency across edges |

### Property Cleanup Functions

Each property has a corresponding `clear_*()` function to free memory:

```cpp
mesh->clear_vertices();      // Clear vertex positions
mesh->clear_faces();         // Clear face indices
mesh->clear_tstrips();       // Clear triangle strips
mesh->clear_grid();          // Clear range grid
mesh->clear_colors();        // Clear per-vertex colors
mesh->clear_confidences();   // Clear confidence values
mesh->clear_flags();         // Clear per-vertex flags
mesh->clear_normals();       // Clear computed normals
mesh->clear_curvatures();    // Clear curvatures (pdir1, pdir2, curv1, curv2)
mesh->clear_dcurv();         // Clear curvature derivatives
mesh->clear_pointareas();    // Clear area values
mesh->clear_bbox();          // Clear bounding box
mesh->clear_bsphere();       // Clear bounding sphere
mesh->clear_neighbors();     // Clear neighbor lists
mesh->clear_adjacentfaces(); // Clear adjacent face lists
mesh->clear_across_edge();   // Clear across-edge adjacency
mesh->clear();               // Clear everything
```

### Mesh Utility Methods

| Method | Description |
|--------|-------------|
| `is_bdy(v)` | Returns true if vertex `v` is on the mesh boundary |
| `centroid(f)` | Returns the centroid of face `f` |
| `trinorm(f)` | Returns the normal of face `f` |
| `cornerangle(i, j)` | Returns the angle at corner `j` of face `i` (j in 0-2) |
| `dihedral(i, j)` | Returns dihedral angle between face `i` and its neighbor across edge `j` |

### Statistics

```cpp
// Available statistical operations (StatOp enum)
STAT_MIN, STAT_MINABS, STAT_MAX, STAT_MAXABS,
STAT_SUM, STAT_SUMABS, STAT_SUMSQR,
STAT_MEAN, STAT_MEANABS, STAT_RMS,
STAT_MEDIAN, STAT_STDEV

// Available statistical values (StatVal enum)
STAT_VALENCE, STAT_FACEAREA, STAT_ANGLE,
STAT_DIHEDRAL, STAT_EDGELEN, STAT_X, STAT_Y, STAT_Z

float val = mesh->stat(trimesh::TriMesh::STAT_MEAN, trimesh::TriMesh::STAT_EDGELEN);
float feature_size = mesh->feature_size();
```

### Debugging and Logging

```cpp
// Control verbosity (0 = quiet, higher = more output)
trimesh::TriMesh::verbose = 1;
trimesh::TriMesh::set_verbose(1);

// Custom printf hooks for debug/fatal output
trimesh::TriMesh::set_dprintf_hook(my_printf);
trimesh::TriMesh::set_eprintf_hook(my_error_printf);
```

### Color Mapping for Visualization

The `colors` vector stores per-vertex RGB colors. Colors are commonly used to visualize scalar fields (curvature, distance, etc.) through color mapping.

```cpp
mesh->colors.resize(mesh->vertices.size());
```

#### Assigning Solid Colors

```cpp
// Assign solid red color to all vertices
mesh->colors.resize(mesh->vertices.size());
for (size_t i = 0; i < mesh->vertices.size(); ++i) {
    mesh->colors[i] = trimesh::Color(1.0f, 0.0f, 0.0f);  // RGB (0-1 range)
}

// Using Color static methods
mesh->colors[i] = trimesh::Color::white();
mesh->colors[i] = trimesh::Color::black();
mesh->colors[i] = trimesh::Color::gray(0.5f);
```

#### Color Mapping Strategies

**1. Simple Gradient (Single Value)**

Map a scalar value to a two-color gradient:

```cpp
float min_val = *std::min_element(mesh->curv1.begin(), mesh->curv1.end());
float max_val = *std::max_element(mesh->curv1.begin(), mesh->curv1.end());

for (size_t i = 0; i < mesh->vertices.size(); ++i) {
    float t = (mesh->curv1[i] - min_val) / (max_val - min_val);  // 0 to 1
    mesh->colors[i] = trimesh::Color(t, 0.5f, 1.0f - t);  // Blue to Red
}
```

**2. Diverging Gradient (Positive and Negative)**

For signed values (like mean curvature):

```cpp
float abs_max = std::max(std::abs(min_val), std::abs(max_val));

for (size_t i = 0; i < mesh->vertices.size(); ++i) {
    float t = mesh->curv1[i] / abs_max;  // -1 to 1
    mesh->colors[i] = trimesh::Color(0.5f + 0.5f * t, 0.5f - 0.5f * t, 0.5f);
    // Blue (negative) -> White (zero) -> Red (positive)
}
```

**3. HSV Color Space (Rainbow Gradient)**

Use HSV for smooth rainbow transitions:

```cpp
float min_c = *std::min_element(mesh->curv1.begin(), mesh->curv1.end());
float max_c = *std::max_element(mesh->curv1.begin(), mesh->curv1.end());

for (size_t i = 0; i < mesh->vertices.size(); ++i) {
    float hue = (mesh->curv1[i] - min_c) / (max_c - min_c);  // 0 to 1
    mesh->colors[i] = trimesh::Color::hsv(hue, 1.0f, 1.0f);
}
```

**4. Jet Colormap**

Smooth transition through blue -> cyan -> green -> yellow -> red:

```cpp
float min_val = *std::min_element(mesh->curv1.begin(), mesh->curv1.end());
float max_val = *std::max_element(mesh->curv1.begin(), mesh->curv1.end());

for (size_t i = 0; i < mesh->vertices.size(); ++i) {
    float t = (mesh->curv1[i] - min_val) / (max_val - min_val);

    float r, g, b;
    if (t < 0.5f) {
        r = 0.0f; g = 4.0f * t; b = 1.0f;  // Blue to Cyan/Green
    } else {
        r = 4.0f * (t - 0.5f); g = 1.0f; b = 1.0f - r;  // Yellow to Red
    }

    mesh->colors[i] = trimesh::Color(r, g, b);
}
```

**5. Custom Curvature Color Map**

Visualize curvature direction with color:

```cpp
mesh->need_curvatures();

for (size_t i = 0; i < mesh->vertices.size(); ++i) {
    float k1 = mesh->curv1[i];
    float k2 = mesh->curv2[i];
    float mean = 0.5f * (k1 + k2);
    float gauss = k1 * k2;

    float h = atan2(mean, gauss) / M_PIf + 0.5f;  // Hue based on curvature type
    float s = std::min(1.0f, std::sqrt(k1*k1 + k2*k2));  // Saturation from magnitude
    mesh->colors[i] = trimesh::Color::hsv(h, s, 1.0f);
}
```

#### Full Example

```cpp
#include "TriMesh.h"
#include "Color.h"
#include <algorithm>
#include <cmath>

int main() {
    trimesh::TriMesh *mesh = trimesh::TriMesh::read("input.ply");
    if (!mesh) return 1;

    mesh->need_curvatures();

    mesh->colors.resize(mesh->vertices.size());

    float min_c = *std::min_element(mesh->curv1.begin(), mesh->curv1.end());
    float max_c = *std::max_element(mesh->curv1.begin(), mesh->curv1.end());

    for (size_t i = 0; i < mesh->vertices.size(); ++i) {
        float t = (mesh->curv1[i] - min_c) / (max_c - min_c);
        mesh->colors[i] = trimesh::Color::hsv(t, 1.0f, 1.0f);
    }

    mesh->write("colored.ply");
    return 0;
}
```

## Mesh Algorithms (`TriMesh_algo.h`)

The library provides a comprehensive set of mesh processing algorithms.

### Topology Operations

| Function | Description |
|----------|-------------|
| `edgeflip(mesh)` | Optimally re-triangulate by edge flips |
| `faceflip(mesh)` | Flip vertex order in each face (reverses orientation) |
| `remove_vertices(mesh, toremove)` | Remove specified vertices |
| `remove_unused_vertices(mesh)` | Remove vertices not referenced by any face |
| `remove_faces(mesh, toremove)` | Remove specified faces |
| `remove_sliver_faces(mesh)` | Remove degenerate sliver faces |
| `remap_verts(mesh, remap_table)` | Remap vertices according to index table |
| `reorder_verts(mesh)` | Reorder vertices by traversal order |
| `orient(mesh)` | Consistently orient face normals |
| `erode(mesh)` | Remove boundary vertices and attached faces |

### Smoothing and Filtering

```cpp
#include "TriMesh_algo.h"

// Umbrella operator smoothing (one iteration)
trimesh::umbrella(mesh, stepsize, tangent);

// Taubin lambda/mu smoothing
trimesh::lmsmooth(mesh, 3);  // 3 iterations

// Umbrella operator on normals
trimesh::numbrella(mesh, stepsize);

// Gaussian smoothing
trimesh::smooth_mesh(mesh, sigma);

// Bilateral smoothing (preserves edges)
trimesh::bilateral_smooth_mesh(mesh, sigma1, sigma2);

// Add noise
trimesh::noisify(mesh, amount);
```

### Subdivision

```cpp
enum SubdivScheme {
    SUBDIV_PLANAR,
    SUBDIV_LOOP,
    SUBDIV_LOOP_ORIG,
    SUBDIV_LOOP_NEW,
    SUBDIV_BUTTERFLY,
    SUBDIV_BUTTERFLY_MODIFIED
};

trimesh::subdiv(mesh, SUBDIV_LOOP);
```

### Diffusion Operations

Diffuse per-vertex scalar/vector fields across the mesh:

```cpp
// Diffuse any vector field
trimesh::diffuse_vector(mesh, field, sigma);

// Diffuse normals
trimesh::diffuse_normals(mesh, sigma);

// Diffuse curvatures
trimesh::diffuse_curv(mesh, sigma);

// Diffuse curvature derivatives
trimesh::diffuse_dcurv(mesh, sigma);
```

### Curvature Operations

```cpp
// Diagonalize curvature tensor to principal directions
vec old_u, old_v, new_norm;
vec pdir1, pdir2;
float ku, kuv, kv, k1, k2;
trimesh::diagonalize_curv(old_u, old_v, ku, kuv, kv, new_norm, pdir1, pdir2, k1, k2);

// Reproject curvature between different bases
trimesh::proj_curv(old_u, old_v, old_ku, old_kuv, old_kv, new_u, new_v, new_ku, new_kuv, new_kv);
trimesh::proj_dcurv(old_u, old_v, old_dcurv, new_u, new_v, new_dcurv);
```

### Geometric Transformations

```cpp
// Apply transformation matrix
trimesh::apply_xform(mesh, xf);

// Translation
trimesh::trans(mesh, vec(1.0f, 0.0f, 0.0f));

// Rotation (radians around axis)
trimesh::rot(mesh, M_PI, vec(0.0f, 1.0f, 0.0f));

// Isotropic scaling
trimesh::scale(mesh, 2.0f);

// Anisotropic scaling
trimesh::scale(mesh, 2.0f, 1.0f, 0.5f);

// Scaling along a direction
trimesh::scale(mesh, 1.5f, vec(1.0f, 0.0f, 0.0f));

// Offset surface (inflation)
trimesh::inflate(mesh, 0.1f);

// Clip to bounding box
trimesh::clip(mesh, box);

// Transform the mesh
trimesh::xform xf = trimesh::xform::rot(M_PI/4, vec(0,1,0)) * trimesh::xform::trans(1,0,0);
trimesh::apply_xform(mesh, xf);
```

### Offset Surface

```cpp
// Create an offset surface at a distance
trimesh::inflate(mesh, 0.05f);  // Expand by 0.05 units
```

### Center of Mass and Covariance

```cpp
// Point cloud center of mass
trimesh::point com = trimesh::point_center_of_mass(points);

// Mesh center of mass (area-weighted)
trimesh::point com = trimesh::mesh_center_of_mass(mesh);

// Point cloud covariance matrix
float C[3][3];
trimesh::point_covariance(points, C);

// Mesh covariance (face-weighted)
trimesh::mesh_covariance(mesh, C);
```

### Normalization

```cpp
// Normalize variance to unit RMS distance from center of mass
trimesh::normalize_variance(mesh);

// PCA-based rotation (aligns principal axes with XYZ)
trimesh::pca_rotate(mesh);

// PCA with 90-degree snapping
trimesh::pca_snap(mesh);
```

### Connected Components

```cpp
std::vector<int> comps;      // Maps each face to its component ID
std::vector<int> compsizes;   // Size of each component

// Find components (vertex-connected vs edge-connected)
trimesh::find_comps(mesh, comps, compsizes, conn_vert);

// Select specific component
trimesh::select_comp(mesh, comps, component_id);

// Select largest components
trimesh::select_big_comps(mesh, comps, compsizes, min_size, max_count);

// Select smallest components
trimesh::select_small_comps(mesh, comps, compsizes, max_size, max_count);
```

### Overlap and Registration

```cpp
float area, rmsdist;
trimesh::find_overlap(mesh1, mesh2, area, rmsdist);
trimesh::find_overlap(mesh1, mesh2, xf1, xf2, area, rmsdist);
trimesh::find_overlap(mesh1, mesh2, xf1, xf2, kd1, kd2, area, rmsdist);

// Intersection over union
float iou_val = trimesh::iou(mesh1, mesh2);
float iou_val = trimesh::iou(mesh1, mesh2, xf1, xf2);
float iou_val = trimesh::iou(mesh1, mesh2, xf1, xf2, kd1, kd2);

// Share vertices across components within tolerance
trimesh::shared(mesh, tolerance);

// Join multiple meshes (with optional vertex sharing)
std::vector<TriMesh*> meshes = {mesh1, mesh2};
TriMesh *joined = trimesh::join(meshes, tolerance);
```

### Primitive Generation

```cpp
// Plane (tessellation in X and Y)
TriMesh *plane = trimesh::make_plane(20);              // 20x20 tessellation
TriMesh *plane = trimesh::make_plane(20, 30);          // 20x30 tessellation

// Gaussian bump
TriMesh *bump = trimesh::make_bump(30, sigma);

// Sine wave
TriMesh *wave = trimesh::make_wave(40, omega);

// Fractal landscape
TriMesh *frac = trimesh::make_frac(128);

// Cube
TriMesh *cube = trimesh::make_cube(tessellation);

// Disc
TriMesh *disc = trimesh::make_disc(tess_angle, tess_radius);

// Cylinders
TriMesh *cyl = trimesh::make_cyl(tess_th, tess_h, radius);        // Open cylinder
TriMesh *ccyl = trimesh::make_ccyl(tess_th, tess_h, radius);       // Capped cylinder
TriMesh *scyl = trimesh::make_scyl(tess_th, tess_h, radius);      // Cylinder with hemispherical caps

// Cones
TriMesh *cone = trimesh::make_cone(tess_th, tess_r, r);            // Open cone
TriMesh *ccone = trimesh::make_ccone(tess_th, tess_r, r);           // Capped cone

// Torus
TriMesh *torus = trimesh::make_torus(tess_th, tess_ph, minor_radius);

// Trefoil knot
TriMesh *knot = trimesh::make_knot(tess_th, tess_ph, minor_radius);

// Klein bottle
TriMesh *klein = trimesh::make_klein(tess_th, tess_ph);

// Helix
TriMesh *helix = trimesh::make_helix(tess_th, tess_ph, turns, minor_radius);

// Spheres
TriMesh *sphere = trimesh::make_sphere_polar(tess_ph, tess_th);   // Lat/long tessellation
TriMesh *sphere = trimesh::make_sphere_subdiv(nfaces, nsubdiv);   // Subdivision from Platonic solid

// Surface of revolution
std::vector<trimesh::point> curve = ...;
TriMesh *surf = trimesh::make_surface_of_revolution(tess_th, curve);

// Platonic solids (with specific face counts)
TriMesh *platonic = trimesh::make_platonic(4);   // Tetrahedron (4 faces)
TriMesh *platonic = trimesh::make_platonic(6);   // Cube (6 faces)
TriMesh *platonic = trimesh::make_platonic(8);   // Octahedron (8 faces)
TriMesh *platonic = trimesh::make_platonic(12);  // Dodecahedron (12 faces)
TriMesh *platonic = trimesh::make_platonic(20);  // Icosahedron (20 faces)

// Pre-defined shapes
enum FixedShape {
    // Platonic solids
    SHAPE_TETRAHEDRON, SHAPE_CUBE, SHAPE_OCTAHEDRON,
    SHAPE_DODECAHEDRON, SHAPE_ICOSAHEDRON,
    // Archimedean solids
    SHAPE_TRUNCATED_TETRAHEDRON, SHAPE_CUBOCTAHEDRON, SHAPE_TRUNCATED_CUBE,
    SHAPE_TRUNCATED_OCTAHEDRON, SHAPE_RHOMBICUBOCTAHEDRON,
    SHAPE_TRUNCATED_CUBOCTAHEDRON, SHAPE_ICOSIDODECAHEDRON,
    SHAPE_TRUNCATED_DODECAHEDRON, SHAPE_TRUNCATED_ICOSAHEDRON,
    SHAPE_SNUB_CUBE, SHAPE_RHOMBICOSIDODECAHEDRON,
    SHAPE_TRUNCATED_ICOSIDODECAHEDRON, SHAPE_SNUB_DODECAHEDRON,
    // Catalan solids (duals)
    SHAPE_TRIAKIS_TETRAHEDRON, SHAPE_RHOMBIC_DODECAHEDRON,
    SHAPE_TRIAKIS_OCTAHEDRON, SHAPE_TETRAKIS_HEXAHEDRON,
    SHAPE_DELTOIDAL_ICOSITETRAHEDRON, SHAPE_DISDYAKIS_DODECAHEDRON,
    SHAPE_RHOMBIC_TRIACONTAHEDRON, SHAPE_TRIAKIS_ICOSAHEDRON,
    SHAPE_PENTAKIS_DODECAHEDRON, SHAPE_PENTAGONAL_ICOSITETRAHEDRON,
    SHAPE_DELTOIDAL_HEXECONTAHEDRON, SHAPE_DISDYAKIS_TRIACONTAHEDRON,
    SHAPE_PENTAGONAL_HEXECONTAHEDRON
};

TriMesh *shape = trimesh::make_fixed_shape(SHAPE_ICOSAHEDRON);

// Teapot!
TriMesh *teapot = trimesh::make_teapot(tessellation, omit_bottom, taller);
```

## KD-Tree Search (`KDtree.h`)

The `KDtree` class provides fast nearest-neighbor search for points.

```cpp
#include "KDtree.h"

// Build from vector of points
std::vector<trimesh::point> points = ...;
trimesh::KDtree tree(points);

// Find closest point
trimesh::point query_point(1.0, 2.0, 3.0);
const float *closest = tree.closest_to_pt(query_point);

// With maximum distance constraint
const float *closest = tree.closest_to_pt(query_point, maxdist2);

// With approximation epsilon (returns point within factor of 1+eps of closest)
const float *closest = tree.closest_to_pt(query_point, maxdist2, approx_eps);

// Find closest point to a ray
const float *closest = tree.closest_to_ray(point_on_ray, ray_direction);

// Find k nearest neighbors
std::vector<const float *> knn;
tree.find_k_closest_to_pt(knn, k, query_point, maxdist2, nullptr, approx_eps);

// Check if any point exists within distance
bool exists = tree.exists_pt_within(query_point, maxdist);
```

### Custom Compatibility Filtering

```cpp
struct MyFilter : trimesh::KDtree::CompatFunc {
    bool operator()(const float *p) const override {
        // Custom compatibility check
        return p[2] > 0.0f;  // Example: only consider points above z=0
    }
};

MyFilter filter;
const float *closest = tree.closest_to_pt(query_point, maxdist2, &filter);
```

## ICP Alignment (`ICP.h`)

The library provides tools for aligning two meshes using the Iterative Closest Point (ICP) algorithm.

### Transformation Types

```cpp
enum ICP_xform_type {
    ICP_TRANSLATION,  // Translation only
    ICP_RIGID,       // Rotation + translation (default)
    ICP_SIMILARITY,  // Rotation + translation + uniform scale
    ICP_AFFINE       // Full affine transformation
};
```

### Basic ICP Interface

```cpp
#include "ICP.h"

trimesh::TriMesh *mesh1 = ...;
trimesh::TriMesh *mesh2 = ...;

// Build kd-trees for efficient correspondence search
trimesh::KDtree *kd1 = new trimesh::KDtree(mesh1->vertices);
trimesh::KDtree *kd2 = new trimesh::KDtree(mesh2->vertices);

// Set initial transforms
trimesh::xform xf1 = trimesh::xform::identity();
trimesh::xform xf2 = trimesh::xform::identity();

// Run ICP (aligns mesh2 to mesh1)
float err = trimesh::ICP(mesh1, mesh2, xf1, xf2, kd1, kd2, verbose, ICP_RIGID);

// Apply result to mesh2
mesh2->need_neighbors();  // For proper transformation
trimesh::apply_xform(mesh2, xf2);
```

### Advanced ICP Interface

```cpp
// With weights for outlier rejection
std::vector<float> weights1, weights2;
float err = trimesh::ICP(mesh1, mesh2, xf1, xf2, kd1, kd2,
                         weights1, weights2, maxdist, verbose, ICP_SIMILARITY);

// Automatically determine maxdist
float maxdist = 0.0f;  // Set to 0 to auto-compute
```

### Overlap Computation

```cpp
// Create acceleration grid
trimesh::Grid *grid1 = trimesh::make_grid(mesh1);
trimesh::Grid *grid2 = trimesh::make_grid(mesh2);

// Compute overlaps
std::vector<float> o1, o2;
float maxdist = 0.0f;
trimesh::compute_overlaps(mesh1, mesh2, xf1, xf2, kd1, kd2,
                          grid1, grid2, o1, o2, maxdist, verbose);
```

## Transformations (`XForm.h`)

The `XForm` class represents 4x4 transformation matrices (column-major order, compatible with OpenGL).

```cpp
#include "XForm.h"

// Default is 4x4 double-precision transformation
typedef XForm<double> xform;
typedef XForm<double> dxform;
typedef XForm<float> fxform;

// Identity transformation
xform xf = xform::identity();
```

### Creating Transformations

```cpp
// Translation
xform xf = xform::trans(1.0, 2.0, 3.0);
xform xf = xform::trans(vec(1.0, 2.0, 3.0));

// Rotation (angle in radians)
xform xf = xform::rot(M_PI, 0.0, 1.0, 0.0);  // 180 degrees around Y
xform xf = xform::rot(M_PI/2, vec(1.0, 0.0, 0.0));

// Rotation that maps one direction to another
xform xf = xform::rot_into(0.0, 0.0, 1.0, 1.0, 0.0, 0.0);

// Scaling
xform xf = xform::scale(2.0);                    // Uniform scale
xform xf = xform::scale(2.0, 1.0, 0.5);           // Non-uniform scale
xform xf = xform::scale(1.5, 1.0, 0.0, 0.0);      // Along a direction

// OpenGL-style projections
xform xf = xform::ortho(left, right, bottom, top, near, far);
xform xf = xform::frustum(left, right, bottom, top, near, far);

// Outer product
xform xf = xform::outer(vec_a, vec_b);

// From array
float A[3][3] = {{...}};
xform xf = xform::fromarray(A);
float B[4][4] = {{...}};
xform xf = xform::fromarray(B);
```

### Accessing Elements

```cpp
xform xf = ...;

// By index (column-major)
xf[0];   // Row 0, Col 0
xf[4];   // Row 0, Col 1
xf[12];  // Row 0, Col 3

// By row/column (1-indexed for compatibility)
xf(1, 2);  // Row 1, Col 2
xf(4, 4);  // Row 4, Col 4
```

### Matrix Operations

```cpp
// Multiplication
xform result = xf1 * xf2;

// Inverse
xform invXf = inv(xf);
invert(xf);  // In-place

// Transpose
xform transXf = transp(xf);
transpose(xf);  // In-place

// Extracting parts
xform rotOnly = rot_only(xf);   // Upper 3x3 rotation matrix
xform transOnly = trans_only(xf);  // Translation column

// Normal transformation (inverse transpose, for normals)
xform normXf = norm_xf(xf);

// Decompose rotation to angle and axis
float angle;
Vec3 axis;
decompose_rot(xf, angle, axis);

// Make orthonormal (remove scaling)
orthogonalize(xf);

// Interpolation (screw decomposition)
xform interp = mix(xf1, xf2, 0.5f);  // 50% from xf1 to xf2
```

### File I/O

```cpp
// Read transform from file
xform xf;
xf.read("transform.xf");

// Write transform to file
xf.write("transform.xf");

// Generate .xf filename from mesh filename
std::string xf_file = trimesh::xfname("mesh.ply");  // Returns "mesh.xf"
```

### Matrix-Vector Multiplication

```cpp
xform xf = ...;
Vec3 point(1.0, 2.0, 3.0);
Vec3 transformed = xf * point;  // Applies transformation
```

### Usage with OpenGL

```cpp
xform xf = ...;
glMultMatrixd(xf);  // Direct upload to OpenGL (column-major)
```

## GUI Development with `gluit`

The `gluit` library simplifies building OpenGL-based applications for mesh visualization.

### Using `GLCamera`

The `GLCamera` class handles 3D viewport manipulation with trackball/arcball interaction.

```cpp
#include "GLCamera.h"

trimesh::GLCamera camera;

// Initialize GL settings for viewing a scene
trimesh::point scene_center(0, 0, 0);
float scene_size = 100.0f;
camera.setupGL(scene_center, scene_size);

// Mouse input handling
// Mouse buttons: NONE, ROTATE, MOVEXY, MOVEZ, WHEELUP, WHEELDOWN, LIGHT
int mousex = ..., mousey = ...;
trimesh::Mouse::button b = trimesh::Mouse::ROTATE;

xform xf = xform::identity();  // Current modelview transform
camera.mouse(mousex, mousey, b, scene_center, scene_size, xf);

// Auto-spin animation
if (camera.autospin(xf)) {
    // Redraw needed
}

// Stop spinning
camera.stopspin();
```

### Camera Constraints

```cpp
// Constrain camera movement
camera.set_constraint(trimesh::GLCamera::UNCONSTRAINED);
camera.set_constraint(trimesh::GLCamera::XCONSTRAINED);
camera.set_constraint(trimesh::GLCamera::YCONSTRAINED);
camera.set_constraint(trimesh::GLCamera::ZCONSTRAINED);

// Query constraint
trimesh::GLCamera::Constraint c = camera.constraint();
```

### Lighting

```cpp
// Get light direction
trimesh::vec light = camera.light();

// Set light direction
camera.set_light(trimesh::vec(0.5f, 0.5f, 1.0f));
```

### Field of View

```cpp
// Get/set field of view
float fov = camera.fov();
camera.set_fov(0.8f);  // radians
```

## Command-Line Utilities

A suite of pre-built tools is available in the `bin/` directory after building:

| Utility | Description |
|---------|-------------|
| `mesh_view` | Interactive 3D mesh viewer |
| `mesh_align` | Align two or more meshes |
| `mesh_filter` | Apply filters (smoothing, noise removal, etc.) |
| `mesh_info` | Display mesh statistics and information |
| `mesh_make` | Create primitive meshes |
| `mesh_cat` | Concatenate multiple meshes |
| `mesh_cc` | Connected components |
| `mesh_check` | Check mesh validity |
| `mesh_shade` | Apply shading |
| `mesh_crunch` | Simplify meshes |
| `mesh_hf` | Heightfield operations |
| `grid_subsamp` | Subsample range grids |

### Example Usage

```bash
# View a mesh
./bin/mesh_view model.ply

# Get mesh information
./bin/mesh_info model.ply

# Align two meshes
./bin/mesh_align reference.ply input.ply -o aligned.ply

# Apply smoothing filter
./bin/mesh_filter model.ply --smooth 3 -o smooth.ply

# Create primitives
./bin/mesh_make --sphere 128 -o sphere.ply
./bin/mesh_make --cube 20 -o cube.ply
```

## Troubleshooting

- **Performance**: For large meshes, ensure triangle strips are used for rendering (`need_tstrips()`).
- **Precision**: The library defaults to single-precision floating point. For double precision, you can use `Vec<3, double>` and other templates.
- **Memory**: Use `clear_*()` functions to release memory for properties no longer needed.
- **Segmentation faults**: Ensure `need_*()` functions are called before accessing computed properties like `neighbors`, `adjacentfaces`, etc.
- **Orientation issues**: Call `orient()` if mesh faces have inconsistent winding.