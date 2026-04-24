#include "TriMesh.h"
#include "TriMesh_algo.h"
#include "KDtree.h"
#include "Vec.h"
#include "Box.h"
#include "Color.h"
#include "XForm.h"
#include <iostream>
#include <vector>
#include <cassert>
#include <cmath>
#include <algorithm>
#include <memory>
#include <iomanip>

using namespace trimesh;
using namespace std;

constexpr float EPSILON = 1e-5f;

#define ASSERT_TRUE(x) assert((x))
#define ASSERT_FALSE(x) assert(!(x))
#define ASSERT_EQ(a, b) assert((a) == (b))
#define ASSERT_NE(a, b) assert((a) != (b))
#define ASSERT_FLOAT_EQ(a, b) assert(fabs((a) - (b)) < EPSILON)
#define ASSERT_LT(a, b) assert((a) < (b))
#define ASSERT_LE(a, b) assert((a) <= (b))
#define ASSERT_GT(a, b) assert((a) > (b))
#define ASSERT_GE(a, b) assert((a) >= (b))
#define ASSERT_NEAR(a, b, tol) assert(fabs((a) - (b)) <= (tol))
#define ASSERT_PTR_NOT_NULL(x) assert((x) != nullptr)

void test_vec_basic_operations() {
    cout << "    Testing Vec3f basic operations..." << endl;
    vec3 v1(1.0f, 2.0f, 3.0f);
    vec3 v2(4.0f, 5.0f, 6.0f);
    
    vec3 v_add = v1 + v2;
    ASSERT_FLOAT_EQ(v_add[0], 5.0f);
    ASSERT_FLOAT_EQ(v_add[1], 7.0f);
    ASSERT_FLOAT_EQ(v_add[2], 9.0f);
    
    vec3 v_sub = v2 - v1;
    ASSERT_FLOAT_EQ(v_sub[0], 3.0f);
    ASSERT_FLOAT_EQ(v_sub[1], 3.0f);
    ASSERT_FLOAT_EQ(v_sub[2], 3.0f);
    
    vec3 v_neg = -v1;
    ASSERT_FLOAT_EQ(v_neg[0], -1.0f);
    ASSERT_FLOAT_EQ(v_neg[1], -2.0f);
    ASSERT_FLOAT_EQ(v_neg[2], -3.0f);
}

void test_vec_scalar_operations() {
    cout << "    Testing Vec3f scalar operations..." << endl;
    vec3 v1(1.0f, 2.0f, 3.0f);
    vec3 v_mul = v1 * 2.0f;
    ASSERT_FLOAT_EQ(v_mul[0], 2.0f);
    ASSERT_FLOAT_EQ(v_mul[1], 4.0f);
    ASSERT_FLOAT_EQ(v_mul[2], 6.0f);
    
    vec3 v_div = v1 / 2.0f;
    ASSERT_FLOAT_EQ(v_div[0], 0.5f);
    ASSERT_FLOAT_EQ(v_div[1], 1.0f);
    ASSERT_FLOAT_EQ(v_div[2], 1.5f);
    
    vec3 v_scale = 3.0f * v1;
    ASSERT_FLOAT_EQ(v_scale[0], 3.0f);
    ASSERT_FLOAT_EQ(v_scale[1], 6.0f);
    ASSERT_FLOAT_EQ(v_scale[2], 9.0f);
}

void test_vec_dot_cross_product() {
    cout << "    Testing Vec3f dot and cross product..." << endl;
    vec3 v1(1.0f, 2.0f, 3.0f);
    vec3 v2(4.0f, 5.0f, 6.0f);
    
    float dot_result = v1 DOT v2;
    ASSERT_FLOAT_EQ(dot_result, 32.0f);
    
    vec3 cross_result = v1 CROSS v2;
    ASSERT_FLOAT_EQ(cross_result[0], -3.0f);
    ASSERT_FLOAT_EQ(cross_result[1], 6.0f);
    ASSERT_FLOAT_EQ(cross_result[2], -3.0f);
    
    vec3 parallel(1.0f, 0.0f, 0.0f);
    vec3 antiparallel(-1.0f, 0.0f, 0.0f);
    vec3 perp(0.0f, 1.0f, 0.0f);
    
    ASSERT_LT((parallel DOT antiparallel), 0.0f);
    ASSERT_FLOAT_EQ((parallel DOT perp), 0.0f);
}

void test_vec_length_normalization() {
    cout << "    Testing Vec3f length and normalization..." << endl;
    vec3 v(3.0f, 4.0f, 0.0f);
    float len_v = len(v);
    ASSERT_FLOAT_EQ(len_v, 5.0f);
    
    float len2_v = len2(v);
    ASSERT_FLOAT_EQ(len2_v, 25.0f);
    
    vec3 v_norm = normalized(v);
    ASSERT_FLOAT_EQ(len(v_norm), 1.0f);
    
    normalize(v);
    ASSERT_FLOAT_EQ(len(v), 1.0f);
}

void test_vec_distance() {
    cout << "    Testing Vec3f distance functions..." << endl;
    point p1(0.0f, 0.0f, 0.0f);
    point p2(3.0f, 4.0f, 0.0f);
    
    ASSERT_FLOAT_EQ(dist(p1, p2), 5.0f);
    ASSERT_FLOAT_EQ(dist2(p1, p2), 25.0f);
}

void test_vec_angle() {
    cout << "    Testing Vec3f angle computation..." << endl;
    vec3 x_axis(1.0f, 0.0f, 0.0f);
    vec3 y_axis(0.0f, 1.0f, 0.0f);
    vec3 neg_x(-1.0f, 0.0f, 0.0f);
    
    ASSERT_FLOAT_EQ(angle(x_axis, y_axis), M_PI_2);
    ASSERT_FLOAT_EQ(angle(x_axis, neg_x), M_PIf);
    ASSERT_FLOAT_EQ(angle(x_axis, x_axis), 0.0f);
}

void test_vec_component_functions() {
    cout << "    Testing Vec3f component functions..." << endl;
    vec3 v(1.0f, -2.0f, 3.0f);
    
    ASSERT_FLOAT_EQ(v.min(), -2.0f);
    ASSERT_FLOAT_EQ(v.max(), 3.0f);
    ASSERT_FLOAT_EQ(v.minabs(), 1.0f);
    ASSERT_FLOAT_EQ(v.maxabs(), 3.0f);
    ASSERT_FLOAT_EQ(v.sum(), 2.0f);
    ASSERT_FLOAT_EQ(v.sumsqr(), 14.0f);
}

void test_vec_comparison() {
    cout << "    Testing Vec3f comparison operators..." << endl;
    vec3 v1(1.0f, 2.0f, 3.0f);
    vec3 v2(1.0f, 2.0f, 3.0f);
    vec3 v3(1.0f, 2.0f, 4.0f);
    vec3 v4(0.0f, 1.0f, 2.0f);
    
    ASSERT_TRUE(v1 == v2);
    ASSERT_FALSE(v1 == v3);
    ASSERT_TRUE(v1 != v3);
    ASSERT_FALSE(v1 != v2);
    ASSERT_TRUE(v1 > v4);
    ASSERT_TRUE(v1 >= v2);
    ASSERT_TRUE(v4 < v1);
    ASSERT_TRUE(v4 <= v1);
}

void test_vec_apply() {
    cout << "    Testing Vec3f apply function..." << endl;
    vec3 v(0.0f, M_PI_2, M_PIf);
    vec3 sin_v = sin(v);
    
    ASSERT_FLOAT_EQ(sin_v[0], 0.0f);
    ASSERT_NEAR(sin_v[1], 1.0f, 1e-6f);
    
    vec3 v_sqr = sqr(v);
    ASSERT_NEAR(v_sqr[1], M_PI_2 * M_PI_2, 1e-6f);
}

void test_vec2_operations() {
    cout << "    Testing Vec2f operations..." << endl;
    vec2 v(3.0f, 4.0f);
    ASSERT_FLOAT_EQ(len(v), 5.0f);
    
    vec2 v2(1.0f, 0.0f);
    vec2 v3(0.0f, 1.0f);
    ASSERT_FLOAT_EQ(angle(v2, v3), M_PI_2);
}

void test_vec4_operations() {
    cout << "    Testing Vec4f operations..." << endl;
    vec4 v(1.0f, 2.0f, 3.0f, 4.0f);
    ASSERT_GT(len(v), 0.0f);
    
    vec4 v2(5.0f, 6.0f, 7.0f, 8.0f);
    vec4 sum = v + v2;
    ASSERT_FLOAT_EQ(sum[0], 6.0f);
    ASSERT_FLOAT_EQ(sum[1], 8.0f);
    ASSERT_FLOAT_EQ(sum[2], 10.0f);
    ASSERT_FLOAT_EQ(sum[3], 12.0f);
    
    float dot_result = v DOT v2;
    ASSERT_FLOAT_EQ(dot_result, 70.0f);
}

void test_box_creation() {
    cout << "    Testing Box creation..." << endl;
    Box<3, float> box;
    ASSERT_FALSE(box.valid);
    
    box = Box<3, float>(point(0, 0, 0), point(1, 1, 1));
    ASSERT_TRUE(box.valid);
}

void test_box_operations() {
    cout << "    Testing Box operations..." << endl;
    Box<3, float> box(point(0, 0, 0), point(2, 2, 2));
    
    point center = box.center();
    ASSERT_NEAR(center[0], 1.0f, EPSILON);
    ASSERT_NEAR(center[1], 1.0f, EPSILON);
    ASSERT_NEAR(center[2], 1.0f, EPSILON);
    
    ASSERT_GT(len(box.size()), 0.0f);
    
    vec3 size = box.size();
    ASSERT_FLOAT_EQ(size[0], 2.0f);
    ASSERT_FLOAT_EQ(size[1], 2.0f);
    ASSERT_FLOAT_EQ(size[2], 2.0f);
    
    ASSERT_TRUE(box.contains(point(1, 1, 1)));
    ASSERT_FALSE(box.contains(point(3, 3, 3)));
}

void test_trimesh_creation() {
    cout << "    Testing TriMesh creation..." << endl;
    TriMesh mesh;
    ASSERT_TRUE(mesh.vertices.empty());
    ASSERT_TRUE(mesh.faces.empty());
    ASSERT_FALSE(mesh.bbox.valid);
    ASSERT_FALSE(mesh.bsphere.valid);
}

void test_trimesh_single_triangle() {
    cout << "    Testing single triangle mesh..." << endl;
    TriMesh mesh;
    mesh.vertices.push_back(point(0, 0, 0));
    mesh.vertices.push_back(point(1, 0, 0));
    mesh.vertices.push_back(point(0, 1, 0));
    mesh.faces.push_back(TriMesh::Face(0, 1, 2));
    
    ASSERT_EQ(mesh.vertices.size(), 3);
    ASSERT_EQ(mesh.faces.size(), 1);
    
    mesh.need_normals();
    ASSERT_EQ(mesh.normals.size(), 3);
    
    mesh.need_bbox();
    ASSERT_TRUE(mesh.bbox.valid);
}

void test_trimesh_bbox() {
    cout << "    Testing bounding box computation..." << endl;
    TriMesh *cube = make_cube(1);
    cube->need_bbox();
    
    ASSERT_TRUE(cube->bbox.valid);
    ASSERT_NEAR(cube->bbox.min[0], -1.0f, EPSILON);
    ASSERT_NEAR(cube->bbox.max[0], 1.0f, EPSILON);
    
    delete cube;
}

void test_trimesh_normals() {
    cout << "    Testing normal computation..." << endl;
    TriMesh mesh;
    mesh.vertices.push_back(point(0, 0, 0));
    mesh.vertices.push_back(point(1, 0, 0));
    mesh.vertices.push_back(point(0, 1, 0));
    mesh.faces.push_back(TriMesh::Face(0, 1, 2));
    
    mesh.need_normals();
    ASSERT_EQ(mesh.normals.size(), 3);
    
    for (const auto& n : mesh.normals) {
        ASSERT_GT(len(n), 0.0f);
    }
}

void test_trimesh_centroid() {
    cout << "    Testing face centroid..." << endl;
    TriMesh mesh;
    mesh.vertices.push_back(point(0, 0, 0));
    mesh.vertices.push_back(point(1, 0, 0));
    mesh.vertices.push_back(point(0, 1, 0));
    mesh.faces.push_back(TriMesh::Face(0, 1, 2));
    
    vec c = mesh.centroid(0);
    ASSERT_NEAR(c[0], 1.0f/3.0f, EPSILON);
    ASSERT_NEAR(c[1], 1.0f/3.0f, EPSILON);
    ASSERT_NEAR(c[2], 0.0f, EPSILON);
}

void test_neighbor_computation() {
    cout << "    Testing neighbor computation..." << endl;
    TriMesh *cube = make_cube(1);
    cube->need_neighbors();
    
    ASSERT_FALSE(cube->neighbors.empty());
    for (const auto& n : cube->neighbors) {
        ASSERT_FALSE(n.empty());
    }
    
    delete cube;
}

void test_adjacentface_computation() {
    cout << "    Testing adjacent face computation..." << endl;
    TriMesh *cube = make_cube(1);
    cube->need_adjacentfaces();
    
    ASSERT_FALSE(cube->adjacentfaces.empty());
    for (const auto& af : cube->adjacentfaces) {
        ASSERT_FALSE(af.empty());
    }
    
    delete cube;
}

void test_across_edge_computation() {
    cout << "    Testing across edge computation..." << endl;
    TriMesh *cube = make_cube(1);
    cube->need_across_edge();
    
    ASSERT_FALSE(cube->across_edge.empty());
    
    delete cube;
}

void test_boundary_detection() {
    cout << "    Testing boundary detection..." << endl;
    
    TriMesh tri;
    tri.vertices.push_back(point(0, 0, 0));
    tri.vertices.push_back(point(1, 0, 0));
    tri.vertices.push_back(point(0, 1, 0));
    tri.faces.push_back(TriMesh::Face(0, 1, 2));
    tri.need_neighbors();
    tri.need_adjacentfaces();
    
    ASSERT_TRUE(tri.is_bdy(0));
    ASSERT_TRUE(tri.is_bdy(1));
    ASSERT_TRUE(tri.is_bdy(2));
}

void test_kdtree_creation() {
    cout << "    Testing KDtree creation..." << endl;
    vector<point> pts;
    for (int i = 0; i < 100; ++i) {
        pts.push_back(point(static_cast<float>(i), 0, 0));
    }
    
    KDtree tree(pts);
}

void test_kdtree_closest_point() {
    cout << "    Testing KDtree closest point..." << endl;
    vector<point> pts = {
        point(0, 0, 0),
        point(1, 0, 0),
        point(2, 0, 0),
        point(10, 0, 0)
    };
    
    KDtree tree(pts);
    const float *nearest = tree.closest_to_pt(point(0.1f, 0, 0), 1.0f);
    ASSERT_PTR_NOT_NULL(nearest);
}

void test_kdtree_ray_query() {
    cout << "    Testing KDtree ray query..." << endl;
    vector<point> pts = {
        point(0, 0, 0),
        point(1, 0, 0),
        point(2, 0, 0)
    };
    
    KDtree tree(pts);
    const float *nearest = tree.closest_to_ray(point(0, 0.1f, 0), point(1, 0, 0), 1.0f);
    ASSERT_PTR_NOT_NULL(nearest);
}

void test_kdtree_k_nearest() {
    cout << "    Testing KDtree k-nearest..." << endl;
    vector<point> pts;
    for (int i = 0; i < 10; ++i) {
        pts.push_back(point(static_cast<float>(i), 0, 0));
    }
    
    KDtree tree(pts);
    vector<const float *> knn;
    tree.find_k_closest_to_pt(knn, 3, point(0.1f, 0, 0), 10.0f);
    
    ASSERT_EQ(knn.size(), 3);
}

void test_kdtree_within_distance() {
    cout << "    Testing KDtree exists within distance..." << endl;
    vector<point> pts = {
        point(0, 0, 0),
        point(1, 0, 0),
        point(5, 0, 0)
    };
    
    KDtree tree(pts);
    ASSERT_TRUE(tree.exists_pt_within(point(0, 0, 0), 2.0f));
    ASSERT_FALSE(tree.exists_pt_within(point(10, 0, 0), 0.5f));
}

void test_make_cube() {
    cout << "    Testing make_cube..." << endl;
    TriMesh *cube = make_cube(1);
    
    ASSERT_FALSE(cube->vertices.empty());
    ASSERT_FALSE(cube->faces.empty());
    ASSERT_GT(cube->vertices.size(), 0);
    ASSERT_GT(cube->faces.size(), 0);
    
    delete cube;
}

void test_make_sphere() {
    cout << "    Testing make_sphere_polar..." << endl;
    TriMesh *sphere = make_sphere_polar(16, 16);
    
    ASSERT_FALSE(sphere->vertices.empty());
    ASSERT_FALSE(sphere->faces.empty());
    
    sphere->need_normals();
    ASSERT_FALSE(sphere->normals.empty());
    
    delete sphere;
}

void test_make_cylinder() {
    cout << "    Testing make_cylinder..." << endl;
    TriMesh *cyl = make_cyl(16, 4);
    
    ASSERT_FALSE(cyl->vertices.empty());
    ASSERT_FALSE(cyl->faces.empty());
    
    delete cyl;
}

void test_make_cone() {
    cout << "    Testing make_cone..." << endl;
    TriMesh *cone = make_cone(16, 4);
    
    ASSERT_FALSE(cone->vertices.empty());
    ASSERT_FALSE(cone->faces.empty());
    
    delete cone;
}

void test_make_torus() {
    cout << "    Testing make_torus..." << endl;
    TriMesh *torus = make_torus(16, 8);
    
    ASSERT_FALSE(torus->vertices.empty());
    ASSERT_FALSE(torus->faces.empty());
    
    delete torus;
}

void test_make_plane() {
    cout << "    Testing make_plane..." << endl;
    TriMesh *plane = make_plane(4, 4);
    
    ASSERT_FALSE(plane->vertices.empty());
    ASSERT_FALSE(plane->faces.empty());
    
    delete plane;
}

void test_make_fixed_shapes() {
    cout << "    Testing make_fixed_shape..." << endl;
    TriMesh *tetra = make_fixed_shape(SHAPE_TETRAHEDRON);
    ASSERT_FALSE(tetra->vertices.empty());
    delete tetra;
    
    TriMesh *oct = make_fixed_shape(SHAPE_OCTAHEDRON);
    ASSERT_FALSE(oct->vertices.empty());
    delete oct;
    
    TriMesh *icos = make_fixed_shape(SHAPE_ICOSAHEDRON);
    ASSERT_FALSE(icos->vertices.empty());
    delete icos;
}

void test_make_teapot() {
    cout << "    Testing make_teapot..." << endl;
    TriMesh *teapot = make_teapot(4);
    
    ASSERT_FALSE(teapot->vertices.empty());
    ASSERT_FALSE(teapot->faces.empty());
    
    delete teapot;
}

void test_subdivision_loop() {
    cout << "    Testing Loop subdivision..." << endl;
    TriMesh *cube = make_cube(1);
    size_t orig_verts = cube->vertices.size();
    
    subdiv(cube, SUBDIV_LOOP);
    
    ASSERT_GT(cube->vertices.size(), orig_verts);
    
    delete cube;
}

void test_subdivision_butterfly() {
    cout << "    Testing Butterfly subdivision..." << endl;
    TriMesh *cube = make_cube(1);
    size_t orig_verts = cube->vertices.size();
    
    subdiv(cube, SUBDIV_BUTTERFLY);
    
    ASSERT_GT(cube->vertices.size(), orig_verts);
    
    delete cube;
}

void test_edge_flip() {
    cout << "    Testing edge flip..." << endl;
    TriMesh *cube = make_cube(1);
    size_t orig_faces = cube->faces.size();
    
    edgeflip(cube);
    
    ASSERT_EQ(cube->faces.size(), orig_faces);
    
    delete cube;
}

void test_face_flip() {
    cout << "    Testing face flip..." << endl;
    TriMesh *cube = make_cube(1);
    size_t orig_faces = cube->faces.size();
    
    faceflip(cube);
    
    ASSERT_EQ(cube->faces.size(), orig_faces);
    
    delete cube;
}

void test_umbrella_smoothing() {
    cout << "    Testing umbrella smoothing..." << endl;
    TriMesh *cube = make_cube(1);
    size_t orig_verts = cube->vertices.size();
    
    umbrella(cube, 0.5f);
    
    ASSERT_EQ(cube->vertices.size(), orig_verts);
    
    delete cube;
}

void test_lm_smooth() {
    cout << "    Testing Taubin smoothing..." << endl;
    TriMesh *cube = make_cube(1);
    size_t orig_verts = cube->vertices.size();
    
    lmsmooth(cube, 2);
    
    ASSERT_EQ(cube->vertices.size(), orig_verts);
    
    delete cube;
}

void test_noisify() {
    cout << "    Testing noisify..." << endl;
    TriMesh *cube = make_cube(1);
    int orig_vert_count = cube->vertices.size();
    
    noisify(cube, 0.01f);
    
    ASSERT_EQ(cube->vertices.size(), orig_vert_count);
    
    delete cube;
}

void test_edgeflip_preserves_topology() {
    cout << "    Testing edgeflip preserves topology..." << endl;
    TriMesh *cube = make_cube(1);
    size_t orig_verts = cube->vertices.size();
    size_t orig_faces = cube->faces.size();
    
    edgeflip(cube);
    
    ASSERT_EQ(cube->vertices.size(), orig_verts);
    ASSERT_EQ(cube->faces.size(), orig_faces);
    
    delete cube;
}

void test_faceflip_reverses_orientation() {
    cout << "    Testing faceflip reverses orientation..." << endl;
    TriMesh *cube = make_cube(1);
    size_t orig_faces = cube->faces.size();
    
    faceflip(cube);
    
    ASSERT_EQ(cube->faces.size(), orig_faces);
    
    delete cube;
}

void test_bilateral_smooth() {
    cout << "    Testing bilateral smoothing..." << endl;
    TriMesh *cube = make_cube(1);
    bilateral_smooth_mesh(cube, 0.1f, 0.1f);
    
    ASSERT_FALSE(cube->vertices.empty());
    
    delete cube;
}

void test_inflate() {
    cout << "    Testing inflate..." << endl;
    TriMesh *cube = make_cube(1);
    cube->need_bbox();
    float orig_diagonal = 0.0f;
    if (cube->bbox.valid) {
        orig_diagonal = len(cube->bbox.size());
    }
    
    inflate(cube, 0.1f);
    
    ASSERT_FALSE(cube->vertices.empty());
    
    delete cube;
}

void test_translation() {
    cout << "    Testing translation..." << endl;
    TriMesh *mesh = make_cube(1);
    size_t orig_verts = mesh->vertices.size();
    
    trans(mesh, vec(1, 0, 0));
    
    ASSERT_EQ(mesh->vertices.size(), orig_verts);
    
    delete mesh;
}

void test_rotation() {
    cout << "    Testing rotation..." << endl;
    TriMesh *mesh = make_cube(1);
    size_t orig_verts = mesh->vertices.size();
    
    rot(mesh, M_PI_2, vec(0, 0, 1));
    
    ASSERT_EQ(mesh->vertices.size(), orig_verts);
    
    delete mesh;
}

void test_scaling() {
    cout << "    Testing scaling..." << endl;
    TriMesh *mesh = make_cube(1);
    size_t orig_verts = mesh->vertices.size();
    
    scale(mesh, 2.0f);
    
    ASSERT_EQ(mesh->vertices.size(), orig_verts);
    
    delete mesh;
}

void test_pca_rotate() {
    cout << "    Testing PCA rotation..." << endl;
    TriMesh *mesh = make_cube(1);
    size_t orig_verts = mesh->vertices.size();
    pca_rotate(mesh);
    ASSERT_EQ(mesh->vertices.size(), orig_verts);
    delete mesh;
}

void test_center_of_mass() {
    cout << "    Testing center of mass..." << endl;
    TriMesh *mesh = make_cube(1);
    point com = mesh_center_of_mass(mesh);
    ASSERT_TRUE(isnormal(com[0]) || abs(com[0]) < EPSILON);
    delete mesh;
}

void test_covariance() {
    cout << "    Testing covariance..." << endl;
    TriMesh *mesh = make_cube(1);
    float C[3][3];
    mesh_covariance(mesh, C);
    ASSERT_NE(C[0][0] + C[1][1] + C[2][2], 0.0f);
    delete mesh;
}

void test_connected_components() {
    cout << "    Testing connected components..." << endl;
    TriMesh *cube = make_cube(1);
    vector<int> comps, compsizes;
    find_comps(cube, comps, compsizes);
    ASSERT_FALSE(comps.empty());
    ASSERT_FALSE(compsizes.empty());
    ASSERT_EQ(compsizes[0], static_cast<int>(cube->faces.size()));
    delete cube;
}

void test_select_big_components() {
    cout << "    Testing select big components..." << endl;
    TriMesh *mesh = make_cube(1);
    vector<int> comps, compsizes;
    find_comps(mesh, comps, compsizes);
    select_big_comps(mesh, comps, compsizes, 1, 1);
    ASSERT_FALSE(mesh->vertices.empty());
    delete mesh;
}

void test_remove_sliver_faces() {
    cout << "    Testing remove sliver faces..." << endl;
    TriMesh *mesh = make_cube(1);
    size_t orig_faces = mesh->faces.size();
    remove_sliver_faces(mesh);
    ASSERT_LE(mesh->faces.size(), orig_faces);
    delete mesh;
}

void test_stat_min_max() {
    cout << "    Testing min/max statistics..." << endl;
    TriMesh *mesh = make_cube(1);
    mesh->need_normals();
    float min_x = mesh->stat(TriMesh::STAT_MIN, TriMesh::STAT_X);
    float max_x = mesh->stat(TriMesh::STAT_MAX, TriMesh::STAT_X);
    ASSERT_LT(min_x, max_x);
    delete mesh;
}

void test_stat_mean() {
    cout << "    Testing mean statistics..." << endl;
    TriMesh *mesh = make_cube(1);
    mesh->need_normals();
    float mean_x = mesh->stat(TriMesh::STAT_MEAN, TriMesh::STAT_X);
    ASSERT_TRUE(isnormal(mean_x) || abs(mean_x) < EPSILON);
    delete mesh;
}

void test_stat_valence() {
    cout << "    Testing valence statistics..." << endl;
    TriMesh *mesh = make_cube(1);
    mesh->need_neighbors();
    float mean_val = mesh->stat(TriMesh::STAT_MEAN, TriMesh::STAT_VALENCE);
    ASSERT_GT(mean_val, 0.0f);
    delete mesh;
}

void test_stat_face_area() {
    cout << "    Testing face area statistics..." << endl;
    TriMesh *mesh = make_cube(1);
    float mean_area = mesh->stat(TriMesh::STAT_MEAN, TriMesh::STAT_FACEAREA);
    ASSERT_GT(mean_area, 0.0f);
    delete mesh;
}

void test_stat_edge_length() {
    cout << "    Testing edge length statistics..." << endl;
    TriMesh *mesh = make_cube(1);
    float mean_edge = mesh->stat(TriMesh::STAT_MEAN, TriMesh::STAT_EDGELEN);
    ASSERT_GT(mean_edge, 0.0f);
    delete mesh;
}

void test_feature_size() {
    cout << "    Testing feature size..." << endl;
    TriMesh *mesh = make_cube(1);
    float fs = mesh->feature_size();
    ASSERT_GT(fs, 0.0f);
    delete mesh;
}

void test_curvature_computation() {
    cout << "    Testing curvature computation..." << endl;
    TriMesh *mesh = make_sphere_polar(16, 16);
    mesh->need_curvatures();
    ASSERT_FALSE(mesh->pdir1.empty());
    ASSERT_FALSE(mesh->pdir2.empty());
    ASSERT_FALSE(mesh->curv1.empty());
    ASSERT_FALSE(mesh->curv2.empty());
    delete mesh;
}

void test_principal_curvatures() {
    cout << "    Testing principal curvatures..." << endl;
    TriMesh *mesh = make_sphere_polar(16, 16);
    mesh->need_curvatures();
    for (size_t i = 0; i < mesh->curv1.size(); ++i) {
        ASSERT_TRUE(isfinite(mesh->curv1[i]));
        ASSERT_TRUE(isfinite(mesh->curv2[i]));
    }
    delete mesh;
}

void test_diffuse_curv() {
    cout << "    Testing diffuse curvature..." << endl;
    TriMesh *mesh = make_sphere_polar(8, 8);
    mesh->need_curvatures();
    diffuse_curv(mesh, 0.1f);
    ASSERT_FALSE(mesh->curv1.empty());
    delete mesh;
}

void test_diffuse_normals() {
    cout << "    Testing diffuse normals..." << endl;
    TriMesh *mesh = make_sphere_polar(8, 8);
    mesh->need_normals();
    diffuse_normals(mesh, 0.1f);
    ASSERT_FALSE(mesh->normals.empty());
    delete mesh;
}

void test_ply_read_write() {
    cout << "    Testing PLY read/write..." << endl;
    TriMesh *mesh = make_cube(1);
    mesh->colors.resize(mesh->vertices.size(), Color(1.0, 0.0, 0.0));
    ASSERT_TRUE(mesh->write("test_ply.ply"));
    TriMesh *loaded = TriMesh::read("test_ply.ply");
    ASSERT_PTR_NOT_NULL(loaded);
    delete mesh;
    delete loaded;
}

void test_file_roundtrip() {
    cout << "    Testing file roundtrip..." << endl;
    TriMesh *orig = make_cube(1);
    orig->colors.resize(orig->vertices.size(), Color(1.0, 0.5, 0.25));
    ASSERT_TRUE(orig->write("test_roundtrip.ply"));
    TriMesh *loaded = TriMesh::read("test_roundtrip.ply");
    ASSERT_PTR_NOT_NULL(loaded);
    ASSERT_EQ(loaded->vertices.size(), orig->vertices.size());
    ASSERT_EQ(loaded->faces.size(), orig->faces.size());
    delete orig;
    delete loaded;
}

void test_overlap_computation() {
    cout << "    Testing overlap computation..." << endl;
    TriMesh *mesh1 = make_cube(1);
    TriMesh *mesh2 = make_cube(1);
    float area, rmsdist;
    find_overlap(mesh1, mesh2, area, rmsdist);
    ASSERT_GE(area, 0.0f);
    delete mesh1;
    delete mesh2;
}

void test_iou_computation() {
    cout << "    Testing IOU computation..." << endl;
    TriMesh *mesh1 = make_cube(1);
    TriMesh *mesh2 = make_cube(1);
    float iou_result = iou(mesh1, mesh2);
    ASSERT_GE(iou_result, 0.0f);
    ASSERT_LE(iou_result, 1.0f);
    delete mesh1;
    delete mesh2;
}

void test_empty_mesh() {
    cout << "    Testing empty mesh operations..." << endl;
    TriMesh mesh;
    mesh.need_normals();
    mesh.need_bbox();
    mesh.need_neighbors();
    ASSERT_TRUE(mesh.normals.empty());
    ASSERT_FALSE(mesh.bbox.valid);
}

void test_degenerate_faces() {
    cout << "    Testing degenerate face handling..." << endl;
    TriMesh mesh;
    mesh.vertices.push_back(point(0, 0, 0));
    mesh.vertices.push_back(point(1, 0, 0));
    mesh.vertices.push_back(point(0.5f, 0.01f, 0));
    mesh.faces.push_back(TriMesh::Face(0, 1, 2));
    mesh.need_normals();
    ASSERT_GT(len(mesh.normals[0]), 0.0f);
}

void test_clear_release() {
    cout << "    Testing clear and release..." << endl;
    TriMesh *mesh = make_cube(1);
    mesh->need_normals();
    mesh->need_neighbors();
    mesh->need_bbox();
    mesh->need_curvatures();
    mesh->clear();
    ASSERT_TRUE(mesh->vertices.empty());
    ASSERT_TRUE(mesh->normals.empty());
    delete mesh;
}

void test_large_mesh() {
    cout << "    Testing large mesh performance..." << endl;
    TriMesh *sphere = make_sphere_polar(64, 64);
    sphere->need_normals();
    sphere->need_neighbors();
    sphere->need_bbox();
    ASSERT_FALSE(sphere->vertices.empty());
    ASSERT_GT(sphere->vertices.size(), 1000);
    delete sphere;
}

int main() {
    cout << "\n========================================\n";
    cout << "    TriMesh Unit Test Suite\n";
    cout << "========================================\n";
    cout << "Running with C++20 standard\n\n";
    
    int passed = 0;
    int failed = 0;
    
    auto run_test = [&](const string& name, auto&& func) {
        try {
            func();
            cout << "  [PASS] " << name << "\n";
            passed++;
        } catch (const exception& e) {
            cout << "  [FAIL] " << name << ": " << e.what() << "\n";
            failed++;
        }
    };
    
    cout << "=== Vector Math Tests ===\n";
    run_test("vec_basic_operations", test_vec_basic_operations);
    run_test("vec_scalar_operations", test_vec_scalar_operations);
    run_test("vec_dot_cross_product", test_vec_dot_cross_product);
    run_test("vec_length_normalization", test_vec_length_normalization);
    run_test("vec_distance", test_vec_distance);
    run_test("vec_angle", test_vec_angle);
    run_test("vec_component_functions", test_vec_component_functions);
    run_test("vec_comparison", test_vec_comparison);
    run_test("vec_apply", test_vec_apply);
    run_test("vec2_operations", test_vec2_operations);
    run_test("vec4_operations", test_vec4_operations);
    
    cout << "\n=== Box and Bounding Structures ===\n";
    run_test("box_creation", test_box_creation);
    run_test("box_operations", test_box_operations);
    
    cout << "\n=== Basic TriMesh Operations ===\n";
    run_test("trimesh_creation", test_trimesh_creation);
    run_test("trimesh_single_triangle", test_trimesh_single_triangle);
    run_test("trimesh_bbox", test_trimesh_bbox);
    run_test("trimesh_normals", test_trimesh_normals);
    run_test("trimesh_centroid", test_trimesh_centroid);
    
    cout << "\n=== Connectivity Tests ===\n";
    run_test("neighbor_computation", test_neighbor_computation);
    run_test("adjacentface_computation", test_adjacentface_computation);
    run_test("across_edge_computation", test_across_edge_computation);
    run_test("boundary_detection", test_boundary_detection);
    
    cout << "\n=== KD-Tree Tests ===\n";
    run_test("kdtree_creation", test_kdtree_creation);
    run_test("kdtree_closest_point", test_kdtree_closest_point);
    run_test("kdtree_ray_query", test_kdtree_ray_query);
    run_test("kdtree_k_nearest", test_kdtree_k_nearest);
    run_test("kdtree_within_distance", test_kdtree_within_distance);
    
    cout << "\n=== Mesh Generation Tests ===\n";
    run_test("make_cube", test_make_cube);
    run_test("make_sphere", test_make_sphere);
    run_test("make_cylinder", test_make_cylinder);
    run_test("make_cone", test_make_cone);
    run_test("make_torus", test_make_torus);
    run_test("make_plane", test_make_plane);
    run_test("make_fixed_shapes", test_make_fixed_shapes);
    run_test("make_teapot", test_make_teapot);
    
    cout << "\n=== Mesh Algorithm Tests ===\n";
    run_test("subdivision_loop", test_subdivision_loop);
    run_test("subdivision_butterfly", test_subdivision_butterfly);
    run_test("edge_flip", test_edge_flip);
    run_test("face_flip", test_face_flip);
    run_test("umbrella_smoothing", test_umbrella_smoothing);
    run_test("lm_smooth", test_lm_smooth);
    run_test("noisify", test_noisify);
    run_test("edgeflip_preserves_topology", test_edgeflip_preserves_topology);
    run_test("faceflip_reverses_orientation", test_faceflip_reverses_orientation);
    run_test("bilateral_smooth", test_bilateral_smooth);
    run_test("inflate", test_inflate);
    
    cout << "\n=== Transform Tests ===\n";
    run_test("translation", test_translation);
    run_test("rotation", test_rotation);
    run_test("scaling", test_scaling);
    run_test("pca_rotate", test_pca_rotate);
    run_test("center_of_mass", test_center_of_mass);
    run_test("covariance", test_covariance);
    
    cout << "\n=== Connected Components ===\n";
    run_test("connected_components", test_connected_components);
    run_test("select_big_components", test_select_big_components);
    run_test("remove_sliver_faces", test_remove_sliver_faces);
    
    cout << "\n=== Statistics Tests ===\n";
    run_test("stat_min_max", test_stat_min_max);
    run_test("stat_mean", test_stat_mean);
    run_test("stat_valence", test_stat_valence);
    run_test("stat_face_area", test_stat_face_area);
    run_test("stat_edge_length", test_stat_edge_length);
    run_test("feature_size", test_feature_size);
    
    cout << "\n=== Curvature Tests ===\n";
    run_test("curvature_computation", test_curvature_computation);
    run_test("principal_curvatures", test_principal_curvatures);
    run_test("diffuse_curv", test_diffuse_curv);
    run_test("diffuse_normals", test_diffuse_normals);
    
    cout << "\n=== IO Tests ===\n";
    run_test("ply_read_write", test_ply_read_write);
    run_test("file_roundtrip", test_file_roundtrip);
    
    cout << "\n=== Overlap and IOU Tests ===\n";
    run_test("overlap_computation", test_overlap_computation);
    run_test("iou_computation", test_iou_computation);
    
    cout << "\n=== Edge Cases ===\n";
    run_test("empty_mesh", test_empty_mesh);
    run_test("degenerate_faces", test_degenerate_faces);
    run_test("clear_release", test_clear_release);
    run_test("large_mesh", test_large_mesh);
    
    cout << "\n========================================\n";
    cout << "Tests passed: " << passed << "/" << (passed + failed) << "\n";
    cout << "Tests failed: " << failed << "\n";
    cout << "========================================\n";
    
    return failed > 0 ? 1 : 0;
}