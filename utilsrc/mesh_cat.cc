/*
Szymon Rusinkiewicz
Princeton University

mesh_cat.cc
Concatenate meshes together
*/

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>

#include "TriMesh.h"
#include "TriMesh_algo.h"
using namespace std;
using namespace trimesh;

void usage(const char* myname) {
	fprintf(stderr, "Usage: %s infiles... [-share tol] -o outfile\n", myname);
	exit(1);
}

int main(int argc, char* argv[]) {
	if (argc < 4) usage(argv[0]);

	vector<unique_ptr<TriMesh>> meshes;
	const char* outfile = nullptr;
	float tol = -1.0f;
	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-o") == 0 && i < argc - 1) {
			outfile = argv[i + 1];
			i++;
			continue;
		}
		if (strncmp(argv[i], "-share", 6) == 0 && i < argc - 1) {
			tol = atof(argv[i + 1]);
			i++;
			continue;
		}
		auto m = TriMesh::read(argv[i]);
		if (!m) {
			fprintf(stderr, "Couldn't read file %s\n", argv[i]);
			continue;
		}
		meshes.push_back(std::move(m));
	}

	if (outfile) {
		vector<TriMesh*> raw_meshes;
		for (const auto& m : meshes) raw_meshes.push_back(m.get());
		auto result = std::unique_ptr<TriMesh>(join(raw_meshes, tol));
		if (result) result->write(outfile);
	} else {
		fprintf(stderr, "No output file specified\n");
	}
}
