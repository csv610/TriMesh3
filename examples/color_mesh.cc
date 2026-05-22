#include <cmath>
#include <memory>

#include "Color.h"
#include "TriMesh.h"
#include "TriMesh_algo.h"

int main() {
	auto mesh = trimesh::TriMesh::read("input.ply");
	if (!mesh) return 1;

	mesh->need_normals();
	mesh->need_curvatures();

	mesh->colors.resize(mesh->vertices.size());

	for (size_t i = 0; i < mesh->vertices.size(); ++i) {
		float curv = mesh->curv1[i];
		mesh->colors[i] = trimesh::Color::hsv(fabs(curv) * 3.0f, 1.0f, 1.0f);
	}

	mesh->write("colored.ply");
	return 0;
}