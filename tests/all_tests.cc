#include "TriMesh.h"
#include "TriMesh_algo.h"
#include "KDtree.h"
#include "Vec.h"
#include "ICP.h"
#include <iostream>
#include <vector>
#include <cassert>
#include <cmath>

using namespace trimesh;
using namespace std;

// --- Vector Tests ---
void test_vec() {
	cout << "Testing Vec3f..." << endl;
	vec3 v1(1, 2, 3), v2(4, 5, 6);
	assert((v1 + v2)[0] == 5);
	assert(v1.dot(v2) == 32);
	vec3 v4 = v1.cross(v2);
	assert(v4[0] == -3 && v4[1] == 6 && v4[2] == -3);
	
	vec3 v(3, 4, 0);
	assert(len(v) == 5);
	normalize(v);
	assert(abs(len(v) - 1.0f) < 1e-6);
}

// --- Basic Mesh Tests ---
void test_trimesh_basic() {
	cout << "Testing TriMesh basics..." << endl;
	TriMesh mesh;
	mesh.vertices.push_back(point(0,0,0));
	mesh.vertices.push_back(point(1,0,0));
	mesh.vertices.push_back(point(0,1,0));
	mesh.faces.push_back(TriMesh::Face(0,1,2));
	
	mesh.need_normals();
	assert(mesh.normals.size() == 3);
	mesh.need_bbox();
	assert(mesh.bbox.min[0] == 0 && mesh.bbox.max[0] == 1);
}

// --- KD-tree Tests ---
void test_kdtree() {
	cout << "Testing KDtree..." << endl;
	vector<point> pts = {point(0,0,0), point(1,0,0), point(0.5,0.5,0.5)};
	KDtree tree(pts);
	const float *nearest = tree.closest_to_pt(point(0.1, 0.1, 0.1), 1.0f);
	assert(nearest && nearest[0] == 0);
}

// --- Advanced Algo Tests ---
void test_algo() {
	cout << "Testing Mesh Algorithms..." << endl;
	TriMesh *cube = make_cube(2);
	subdiv(cube, SUBDIV_LOOP);
	umbrella(cube, 0.5f);
	lmsmooth(cube, 2);
	
	vector<int> comps, compsizes;
	find_comps(cube, comps, compsizes);
	assert(!compsizes.empty());
	
	edgeflip(cube);
	faceflip(cube);
	noisify(cube, 0.1f);
	
	delete cube;
}

// --- Stats Tests ---
void test_stats() {
	cout << "Testing Mesh Stats..." << endl;
	TriMesh *m = make_cube(2);
	m->need_normals();
	m->need_adjacentfaces();
	
	float min_x = m->stat(TriMesh::STAT_MIN, TriMesh::STAT_X);
	float mean_v = m->stat(TriMesh::STAT_MEAN, TriMesh::STAT_VALENCE);
	assert(max(min_x, mean_v) != 0); // basic sanity
	
	delete m;
}

// --- IO Tests ---
void test_io() {
	cout << "Testing IO..." << endl;
	TriMesh *cube = make_cube(1);
	cube->colors.resize(cube->vertices.size(), Color(1,0,0));
	cube->write("test_io.ply");
	TriMesh *r = TriMesh::read("test_io.ply");
	assert(r && !r->colors.empty());
	delete cube;
	delete r;
}

int main() {
	test_vec();
	test_trimesh_basic();
	test_kdtree();
	test_algo();
	test_stats();
	test_io();
	cout << "All consolidated tests passed!" << endl;
	return 0;
}
