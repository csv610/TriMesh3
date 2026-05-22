#ifndef TRIMESH_H
#define TRIMESH_H
/*
Szymon Rusinkiewicz
Princeton University

TriMesh.h
Class for triangle meshes.
*/

#include <memory>
#include <mutex>
#include <vector>

#include "Box.h"
#include "Color.h"
#include "Vec.h"
#include "strutil.h"

namespace trimesh {

template <class T>
static inline void clear_and_release(::std::vector<T>& v) {
	// Standard trick to release a vector's storage, since clear() doesn't
	::std::vector<T>().swap(v);
}

/**
 * @brief A class representing a 3D triangle mesh.
 *
 * The TriMesh class provides a comprehensive representation of 3D triangle meshes,
 * supporting various data members for geometry (vertices, faces), appearance (colors, normals),
 * and differential geometry properties (curvatures, principal directions).
 *
 * It employs a lazy computation pattern for many properties. Methods named `need_*()`
 * ensure that the requested data is computed if it hasn't been already.
 *
 * The class also includes cache management for these computed properties and
 * support for mesh connectivity structures.
 */
class TriMesh {
    public:
	//
	// Types
	//
	typedef Vec<3, int> Face;
	typedef Box<3, float> BBox;

	/**
	 * @brief A structure representing a bounding sphere.
	 */
	struct BSphere {
		point center; /**< Center of the sphere. */
		float r;      /**< Radius of the sphere. */
		bool valid;   /**< Whether the sphere has been computed. */
		BSphere() : valid(false) {}
	};

    private:
	mutable std::recursive_mutex computation_mutex;

    public:
	//
	// Copying
	//
	TriMesh(const TriMesh& other)
	    : vertices(other.vertices),
	      faces(other.faces),
	      tstrips(other.tstrips),
	      grid(other.grid),
	      grid_width(other.grid_width),
	      grid_height(other.grid_height),
	      colors(other.colors),
	      confidences(other.confidences),
	      flags(other.flags),
	      flag_curr(other.flag_curr),
	      normals(other.normals),
	      pdir1(other.pdir1),
	      pdir2(other.pdir2),
	      curv1(other.curv1),
	      curv2(other.curv2),
	      dcurv(other.dcurv),
	      cornerareas(other.cornerareas),
	      pointareas(other.pointareas),
	      bbox(other.bbox),
	      bsphere(other.bsphere),
	      neighbors(other.neighbors),
	      adjacentfaces(other.adjacentfaces),
	      across_edge(other.across_edge) {}
	TriMesh& operator=(const TriMesh& other) {
		if (this != &other) {
			vertices = other.vertices;
			faces = other.faces;
			tstrips = other.tstrips;
			grid = other.grid;
			grid_width = other.grid_width;
			grid_height = other.grid_height;
			colors = other.colors;
			confidences = other.confidences;
			flags = other.flags;
			flag_curr = other.flag_curr;
			normals = other.normals;
			pdir1 = other.pdir1;
			pdir2 = other.pdir2;
			curv1 = other.curv1;
			curv2 = other.curv2;
			dcurv = other.dcurv;
			cornerareas = other.cornerareas;
			pointareas = other.pointareas;
			bbox = other.bbox;
			bsphere = other.bsphere;
			neighbors = other.neighbors;
			adjacentfaces = other.adjacentfaces;
			across_edge = other.across_edge;
		}
		return *this;
	}

    public:
	//
	// Enums
	//
	enum TstripRep { TSTRIP_LENGTH, TSTRIP_TERM };
	enum { GRID_INVALID = -1 };
	enum StatOp {
		STAT_MIN,
		STAT_MINABS,
		STAT_MAX,
		STAT_MAXABS,
		STAT_SUM,
		STAT_SUMABS,
		STAT_SUMSQR,
		STAT_MEAN,
		STAT_MEANABS,
		STAT_RMS,
		STAT_MEDIAN,
		STAT_STDEV
	};
	enum StatVal { STAT_VALENCE, STAT_FACEAREA, STAT_ANGLE, STAT_DIHEDRAL, STAT_EDGELEN, STAT_X, STAT_Y, STAT_Z };

	//
	// Constructor
	//
	TriMesh() : grid_width(-1), grid_height(-1), flag_curr(0) {}

	//
	// Members
	//

	/**
	 * @name Geometry
	 * Basic geometric elements of the mesh.
	 * @{
	 */
	::std::vector<point> vertices; /**< List of vertex coordinates. */
	::std::vector<Face> faces;     /**< List of triangles (vertex indices). */
	/** @} */

	/**
	 * @name Alternative representations
	 * @{
	 */
	::std::vector<int> tstrips;  /**< Triangle strips representation. */
	::std::vector<int> grid;     /**< 2D grid representation (for regular meshes). */
	int grid_width, grid_height; /**< Dimensions of the 2D grid. */
	/** @} */

	/**
	 * @name Per-vertex properties
	 * Properties stored or computed at each vertex.
	 * @{
	 */
	::std::vector<Color> colors;	  /**< Per-vertex colors. */
	::std::vector<float> confidences; /**< Per-vertex confidence values. */
	::std::vector<unsigned> flags;	  /**< Per-vertex bitflags. */
	unsigned flag_curr;		  /**< Current flag value for marking. */
	/** @} */

	/**
	 * @name Computed properties
	 * Cached results of lazy-computed properties.
	 * @{
	 */
	::std::vector<vec> normals;	     /**< Surface normals at each vertex. */
	::std::vector<vec> pdir1, pdir2;     /**< Principal curvature directions. */
	::std::vector<float> curv1, curv2;   /**< Principal curvature values. */
	::std::vector<Vec<4, float> > dcurv; /**< Curvature derivatives. */
	::std::vector<vec> cornerareas;	     /**< Per-corner areas. */
	::std::vector<float> pointareas;     /**< Per-vertex areas. */
	/** @} */

	/**
	 * @name Bounding structures
	 * Axis-aligned bounding box and bounding sphere.
	 * @{
	 */
	BBox bbox;	 /**< Axis-aligned bounding box. */
	BSphere bsphere; /**< Bounding sphere. */
	/** @} */

	/**
	 * @name Connectivity structures
	 * Relationships between vertices and faces.
	 * @{
	 */
	::std::vector< ::std::vector<int> > neighbors;	   /**< For each vertex, list of neighboring vertices. */
	::std::vector< ::std::vector<int> > adjacentfaces; /**< For each vertex, list of adjacent faces. */
	/**
	 * @brief For each face, the three faces attached to its edges.
	 *
	 * For example, across_edge[3][2] is the index of the face touching the edge
	 * opposite vertex 2 of face 3.
	 */
	::std::vector<Face> across_edge;
	/** @} */

	//
	// Cache management
	//

	/** @brief Invalidate axis-aligned bounding box cache. */
	void invalidate_bbox() { bbox.clear(); }
	/** @brief Invalidate bounding sphere cache. */
	void invalidate_bsphere() { bsphere.valid = false; }
	/** @brief Invalidate surface normals cache. */
	void invalidate_normals() { normals.clear(); }
	/** @brief Invalidate all connectivity structures. */
	void invalidate_connectivity() {
		neighbors.clear();
		adjacentfaces.clear();
		across_edge.clear();
	}
	/** @brief Invalidate all cached computed properties. */
	void invalidate_cache() {
		invalidate_bbox();
		invalidate_bsphere();
		invalidate_normals();
		invalidate_connectivity();
		curv1.clear();
		curv2.clear();
		pdir1.clear();
		pdir2.clear();
		dcurv.clear();
		pointareas.clear();
		cornerareas.clear();
	}

	//
	// Compute all this stuff...
	//

	/** @brief Ensure faces are available (unpacking from strips or grid if necessary). */
	void need_faces() {
		std::lock_guard<std::recursive_mutex> lock(computation_mutex);
		if (!faces.empty()) return;
		if (!tstrips.empty())
			unpack_tstrips();
		else if (!grid.empty())
			triangulate_grid();
	}

	/** @brief Ensure triangle strips are available. */
	void need_tstrips(TstripRep rep = TSTRIP_LENGTH);
	/** @brief Convert triangle strips between different representations. */
	void convert_strips(TstripRep rep);
	/** @brief Unpack triangle strips into the faces vector. */
	void unpack_tstrips();
	/** @brief Resize the range grid. */
	void resize_grid(int width, int height) {
		grid_width = width;
		grid_height = height;
		grid.clear();
		grid.resize(grid_width * grid_height, GRID_INVALID);
	}
	/** @brief Triangulate the range grid into the faces vector. */
	void triangulate_grid(bool remove_slivers = true);
	/** @brief Ensure surface normals are computed. */
	void need_normals(bool simple_area_weighted = false);
	/** @brief Ensure principal curvatures and directions are computed. */
	void need_curvatures();
	/** @brief Ensure curvature derivatives are computed. */
	void need_dcurv();
	/** @brief Ensure per-vertex and per-corner areas are computed. */
	void need_pointareas();
	/** @brief Ensure the axis-aligned bounding box is computed. */
	void need_bbox();
	/** @brief Ensure the bounding sphere is computed. */
	void need_bsphere();
	/** @brief Ensure vertex neighbor information is computed. */
	void need_neighbors();
	/** @brief Ensure face adjacency information is computed. */
	void need_adjacentfaces();
	/** @brief Ensure face-to-face connectivity via edges is computed. */
	void need_across_edge();

	//
	// Delete everything and release storage
	//
	void clear_vertices() {
		clear_and_release(vertices);
		invalidate_cache();
	}
	void clear_faces() {
		clear_and_release(faces);
		invalidate_cache();
	}
	void clear_tstrips() { clear_and_release(tstrips); }
	void clear_grid() {
		clear_and_release(grid);
		grid_width = grid_height = -1;
	}
	void clear_colors() { clear_and_release(colors); }
	void clear_confidences() { clear_and_release(confidences); }
	void clear_flags() {
		clear_and_release(flags);
		flag_curr = 0;
	}
	void clear_normals() { clear_and_release(normals); }
	void clear_curvatures() {
		clear_and_release(pdir1);
		clear_and_release(pdir2);
		clear_and_release(curv1);
		clear_and_release(curv2);
	}
	void clear_dcurv() { clear_and_release(dcurv); }
	void clear_pointareas() {
		clear_and_release(pointareas);
		clear_and_release(cornerareas);
	}
	void clear_bbox() { bbox.clear(); }
	void clear_bsphere() { bsphere.valid = false; }
	void clear_neighbors() { clear_and_release(neighbors); }
	void clear_adjacentfaces() { clear_and_release(adjacentfaces); }
	void clear_across_edge() { clear_and_release(across_edge); }
	/** @brief Clear all data from the mesh and reset its state. */
	void clear() {
		clear_vertices();
		clear_faces();
		clear_tstrips();
		clear_grid();
		clear_colors();
		clear_confidences();
		clear_flags();
		clear_normals();
		clear_curvatures();
		clear_dcurv();
		clear_pointareas();
		clear_bbox();
		clear_bsphere();
		clear_neighbors();
		clear_adjacentfaces();
		clear_across_edge();
	}

	//
	// Input and output
	//
    protected:
	static bool read_helper(const char* filename, TriMesh* mesh);

    public:
	static ::std::unique_ptr<TriMesh> read(const char* filename);
	static ::std::unique_ptr<TriMesh> read(const ::std::string& filename);
	bool write(const char* filename);
	bool write(const ::std::string& filename);

	//
	// Useful queries
	//

	// Is vertex v on the mesh boundary?
	inline bool is_bdy(int v) {
		if (unlikely(neighbors.empty())) need_neighbors();
		if (unlikely(adjacentfaces.empty())) need_adjacentfaces();
		return neighbors[v].size() != adjacentfaces[v].size();
	}

	// Centroid of face f
	inline vec centroid(int f) {
		if (unlikely(faces.empty())) need_faces();
		return (1.0f / 3.0f) * (vertices[faces[f][0]] + vertices[faces[f][1]] + vertices[faces[f][2]]);
	}

	// Normal of face f
	inline vec trinorm(int f) {
		if (unlikely(faces.empty())) need_faces();
		return trimesh::trinorm(vertices[faces[f][0]], vertices[faces[f][1]], vertices[faces[f][2]]);
	}

	// Angle of corner j in triangle i
	inline float cornerangle(int i, int j) {
		using namespace ::std;

		if (unlikely(faces.empty())) need_faces();
		const point& p0 = vertices[faces[i][j]];
		const point& p1 = vertices[faces[i][NEXT_MOD3(j)]];
		const point& p2 = vertices[faces[i][PREV_MOD3(j)]];
		return acos(dot(p1 - p0, p2 - p0));
	}

	// Dihedral angle between face i and face across_edge[i][j]
	inline float dihedral(int i, int j) {
		if (unlikely(across_edge.empty())) need_across_edge();
		if (unlikely(across_edge[i][j] < 0)) return 0.0f;
		vec mynorm = trinorm(i);
		vec othernorm = trinorm(across_edge[i][j]);
		float ang = angle(mynorm, othernorm);
		vec towards = 0.5f * (vertices[faces[i][NEXT_MOD3(j)]] + vertices[faces[i][PREV_MOD3(j)]]) -
			      vertices[faces[i][j]];
		if (dot(towards, othernorm) < 0.0f)
			return M_PIf + ang;
		else
			return M_PIf - ang;
	}

	// Statistics
	float stat(StatOp op, StatVal val);
	float feature_size();

	//
	// Debugging
	//

	// Debugging printout, controllable by a "verbose"ness parameter
	static int verbose;
	static void set_verbose(int);
	static void (*dprintf_hook)(const char*);
	static void set_dprintf_hook(void (*hook)(const char*));
	static void dprintf(const char* format, ...);

	// Same as above, but fatal-error printout
	static void (*eprintf_hook)(const char*);
	static void set_eprintf_hook(void (*hook)(const char*));
	static void eprintf(const char* format, ...);

	/** @brief Virtual destructor. */
	virtual ~TriMesh() {}
};

}  // namespace trimesh

#endif
