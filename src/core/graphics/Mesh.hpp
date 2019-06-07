
#ifndef __SIBR_MESH_HPP__
# define __SIBR_MESH_HPP__

# include <vector>
# include <map>
# include <sstream>

# include "core/graphics/Config.hpp"
# include "core/system/Vector.hpp"
# include "core/graphics/MeshBufferGL.hpp"
# include "core/graphics/Image.hpp"

///// Be sure to use STL objects from client's dll version by exporting this declaration (see warning C4251)
//template class SIBR_GRAPHICS_EXPORT std::vector<Vector3f>;
//template class SIBR_GRAPHICS_EXPORT std::vector<Vector3u>;

namespace sibr
{

	///
	/// Simple class for storing data about a mesh.
	///
	/// DevNote: Currently this class could be a simple POD struct. However
	/// 'Mesh' should be an important actor for this project and might
	/// evolve. That's why I prefer begin interfaces for it now.
	/// \ingroup sibr_graphics
	class SIBR_GRAPHICS_EXPORT Mesh
	{
	public:
		typedef	std::vector<Vector3f>	Vertices;
		typedef	std::vector<Vector3f>	Normals;
		typedef std::vector<Vector3u>	Triangles;
		typedef	std::vector<Vector3f>	Colors;
		typedef	std::vector<Vector2f>	UVs;
		typedef std::vector<int>		MatIds;
		typedef std::vector<std::string>		MatId2Name;
		typedef std::map<std::string, sibr::ImageRGB::Ptr>		OpacityMaps;

		enum RenderMode
		{
			PointRenderMode,
			LineRenderMode,
			FillRenderMode
		};
		SIBR_CLASS_PTR(Mesh);
	public:

		Mesh( bool withGraphics = true ) :_meshPath("")
		{ if( withGraphics ) _gl.bufferGL.reset( new MeshBufferGL); else _gl.bufferGL = nullptr; }

		/// Set vertices
		inline void	vertices(const Vertices& vertices);
		/// Set vertices from vector of coord (float)
		void vertices( const std::vector<float>& vertices );
		/// Get vertices
		inline const Vertices& vertices( void ) const;

		//get non const accessor to vertices, if gl mesh, means the mesh will be reload to the gpu
		//inline  Vertices& verticesModify(void) ;
		inline void replaceVertice(int vertex_id, const sibr::Vector3f & v) ;

		Mesh::Ptr clone() const;

		/// Get vertices in an array using the following format:
		/// {0x, 0y, 0z, 1x, 1y, 1z, 2x, 2y, 2z, ...}.
		/// Useful for rendering and converting to another mesh
		/// struct.
		inline const float* vertexArray( void ) const;
		//inline std::vector<float> vertexArray( void ) const;

		/// \todo TODO: Rename triangles() into indices()
		/// Set triangles. Each triangle contains 3 indices and
		/// each indice correspond to a vertex in the vertices list.
		inline void	triangles(const Triangles& triangles);
		/// Set triangles. Using vector of uint.
		/// Each triangle contains 3 indices and
		/// each indice correspond to a vertex in the vertices list.
		void	triangles(const std::vector<uint>& triangles);
		/// Get triangles. Each triangle contains 3 indices and
		/// each indice correspond to a vertex in the vertices list.
		inline const Triangles& triangles( void ) const;
		/// Get triangles in an array using the following format:
		/// {0a, 0b, 0c, 1a, 1b, 1c, 2a, 2b, 2c, ...}.
		/// Useful for rendering and converting to another mesh
		/// struct.
		inline const uint* triangleArray( void ) const;
		//inline std::vector<uint> triangleArray( void ) const;

		/// Set colors
		inline void	colors( const Colors& colors );
		/// Get colors
		inline const Colors& colors( void ) const;
		/// Return TRUE if each vertex has a color assigned
		inline bool	hasColors( void ) const;
		/// Get colors in an array using the following format:
		/// {0r, 0g, 0b, 1r, 1g, 1b, 2r, 2g, 2b, ...}.
		/// Useful for rendering and converting to another mesh
		/// struct.
		inline const float* colorArray( void ) const;
		//inline std::vector<float> colorArray( void ) const;

		/// Set texture coordinates (UVs)
		inline void	texCoords( const UVs& texcoords );
		void		texCoords( const std::vector<float>& texcoords );

		/// Get texture coordinates
		inline const UVs& texCoords( void ) const;
		/// Return TRUE if each vertex has a color assigned
		inline bool	hasTexCoords( void ) const;
		/// Get texture coordinate in an array using the following format:
		/// {0u, 0v, 1u, 1v, 2u, 2v, ...}
		/// Useful for rendering and converting to another mesh
		/// struct.
		inline const float* texCoordArray( void ) const;

		/// Set normals
		inline void	normals(const Normals& normals);
		/// Set normals from vector of float
		void	normals(const std::vector<float>& normals);
		/// Get colors
		inline const Normals& normals( void ) const;
		/// Return TRUE if each vertex has a normal assigned
		inline bool	hasNormals( void ) const;
		/// Get normals in an array using the following format:
		/// {0x, 0y, 0z, 1x, 1y, 1z, 2x, 2y, 2z, ...}.
		/// Useful for rendering and converting to another mesh
		/// struct.
		inline const float* normalArray( void ) const;
		//inline std::vector<float> normalArray( void ) const;

		/// Make the mesh whole. ie: it will have default values for all components (texture, materials, colors, etc)
		/// It is usefull when merging two meshes. If the second one is missing some attributes, the merging will break the good mesh.
		void	makeWhole(void);
		/// Generate vertex normals by using the average of
		/// all triangle normals around a each vertex
		void	generateNormals( void );

		/// Generate smooth vertex normals by using the average of
		/// all triangle normals around a each vertex and iterating this process.
		/// Takes also the area of triangles as a weight.
		void	generateSmoothNormals(int numIter);

		/// Generate smooth vertex normals by using the average of
		/// all triangle normals around a each vertex and iterating this process.
		/// Takes also the area of triangles as a weight.
		/// This methods also consider the fact that because of texture coordinates we may have duplicates vertices
		void	generateSmoothNormalsDisconnected(int numIter);

		void laplacianSmoothing(int numIter, bool updateNormals);

		/// Generate a new mesh given a boolean function that
		/// state if a vertex should be kept or not
		
		Mesh generateSubMesh(std::function<bool(int)> func) const;

		/// Load a mesh from the disk.
		/// (tested with .ply, should work with .obj)
		bool	load( const std::string& filename );
		/// Load a mesh from a mitsuba XML files.
		/// It allows handling instances used several times.
		bool	loadMtsXML(const std::string& filename);
		/// Save the proxy as a mesh to the disk (.ply ONLY).
		/// Use \param universal to indicate if you want this mesh to be readable
		/// by most of 3d viewer application (e.g. MeshLab). In this other case,
		/// the mesh will be saved with the best quality of information for the
		/// sibr project.
		void	save( const std::string& filename, bool universal=false ) const;

		/// Save the mesh to .ply file (using the binary version).
		/// Use \param universal to indicate if you want this mesh to be readable
		/// by most of 3d viewer application (e.g. MeshLab). In this other case,
		/// the mesh will be saved with the best quality of information for the
		/// sibr project.
		/// Currently, only vertices and colors are saved.
		bool		saveToBinaryPLY( const std::string& filename, bool universal=false ) const;
		/// Save the mesh to .ply file (using the ascii version).
		/// Use \param universal to indicate if you want this mesh to be readable
		/// by most of 3d viewer application (e.g. MeshLab). In this other case,
		/// the mesh will be saved with the best quality of information for the
		/// sibr project.
		/// Currently, only vertices and colors are saved.
		bool		saveToASCIIPLY( const std::string& filename, bool universal=false, const std::string& textureName="TEXTURE_NAME_TO_PUT_IN_THE_FILE" ) const;

		bool		saveToObj( const std::string& filename, bool universal=false ) const;

		/** GL render function using VBOs */
		void	render(
			bool depthTest = true,
			bool backFaceCulling = true,
			RenderMode mode = FillRenderMode,
			bool frontFaceCulling = false,
			bool invertDepthTest = false,
			bool tessellation = false
		) const;

		/// Render the elements (taken either from the index buffer or directly in the vertex buffer)
		/// from the begin-th to the end-th.
		void	renderSubMesh(unsigned int begin, unsigned int end,
			bool depthTest = true,
			bool backFaceCulling = true,
			RenderMode mode = FillRenderMode,
			bool frontFaceCulling = false,
			bool invertDepthTest = false
		) const;

		void	forceBufferGLUpdate( void ) const;
		void	freeBufferGLUpdate(void) const;

		/// Rendering of vertex as points
		void	render_points(bool depthTest) const;

		void	render_points(void) const;

		void	render_lines(void) const;

		/// Merge another mesh into this one
		void		merge( const Mesh& other );

		void		eraseTriangles(const std::vector<uint>& faceIDList);

		/// Load config from a property file using the given \param key
		//bool	config( const Property& conf, const std::string& key );

		/// get sub mesh with list of vertices
		enum class VERTEX_LIST_CHOICE { KEEP, REMOVE };
		struct SubMesh {
			std::shared_ptr<sibr::Mesh> meshPtr;
			std::vector<int> complementaryVertices; //vertices present in at least one removed triangle, can be used as an arg to extractSubMesh() to get the extrated Mesh
		};
		SubMesh extractSubMesh(const std::vector<int> & vertices, VERTEX_LIST_CHOICE v_choice) const;

		sibr::Mesh invertedFacesMesh() const;

		/// \todo TT TODO clean these 2 functions
		sibr::Mesh::Ptr invertedFacesMesh2() const;

		///
		inline const std::string getMeshFilePath( void ) const;

		Eigen::AlignedBox<float,3>	getBoundingBox( void ) const;

		sibr::Vector3f centroid() const;

		/** IMPORTANT NOTE: This function has not been strongly tested! */
		void						getBoundingSphere(Vector3f& outCenter, float& outRadius, bool referencedOnly=false) const;

		sibr::Vector2f getZnearZfar() const;

		/* to export/load to/from Off file format stream, can be used to convert from/to cgal mesh data structures */
		std::stringstream getOffStream(bool verbose = false) const;

		/// \todo TODO : check for errors */
		void fromOffStream(std::stringstream & stream, bool generateNormals = true );

		/* compute a simple sphere with UVs coordinates to use with lat-long envMap*/
		static sibr::Mesh::Ptr getEnvSphere(sibr::Vector3f center, float radius, sibr::Vector3f zenith, sibr::Vector3f north,
											std::string partOfSphere="whole");

		/* compute a simple cube with normals */
		static sibr::Mesh::Ptr getTestCube(bool withGraphics = true);

		static Mesh::Ptr getSphereMesh(const Vector3f & center, float radius, bool withGraphics = true, int precision = 50);

		sibr::Mesh::Ptr subDivide(float limitSize) const;
		
		float meanEdgeSize() const;

		std::vector<std::vector<int> > removeDisconnectedComponents();

	protected:
		/// used to prevent copying openGL ids
		struct BufferGL
		{
			BufferGL(void) : dirtyBufferGL(true), bufferGL(nullptr) { }

			/// Mimic the mode of the other instance (create a Buffer if the
			/// other use a buffer).
			BufferGL(const BufferGL& other) { operator =(other); }
			BufferGL& operator =(const BufferGL& other) {
				bufferGL.reset(other.bufferGL? new MeshBufferGL() : nullptr);
				dirtyBufferGL = (other.bufferGL!=nullptr);
				return *this;
			}

			bool			dirtyBufferGL;
			std::unique_ptr<MeshBufferGL>	bufferGL;
		};
		public: mutable BufferGL	_gl;

		// Seb: It would be better if MeshBufferGL (and GL stuffs) were outside this class.
		// It should work exactly like Image (CPU, here it's Mesh) and Texture(GPU version
		// of Image, here it's MeshBufferGL)

		Vertices	_vertices;
		Triangles	_triangles;
		Normals		_normals;
		Colors		_colors;
		UVs			_texcoords;

	private:
		std::string _meshPath; //TT can be used to reload the mesh with/without graphics option in constructor

	};

	///// DEFINITION /////

	void	Mesh::vertices( const Vertices& vertices ) {
		_vertices = vertices; _gl.dirtyBufferGL = true;
	}

	const Mesh::Vertices& Mesh::vertices( void ) const {
		return _vertices;
	}

	inline void Mesh::replaceVertice(int vertex_id, const sibr::Vector3f & v)
	{
		if (vertex_id >= 0 && vertex_id < (int)(vertices().size())) {
			_vertices[vertex_id] = v;
		}
	}

	// Mesh::Vertices & Mesh::verticesModify(void)
	//{
	//	_gl.dirtyBufferGL = true;
	//	return _vertices;
	//}

	void	Mesh::triangles( const Triangles& triangles ) {
		_triangles = triangles; _gl.dirtyBufferGL = true;
	}

	const Mesh::Triangles& Mesh::triangles( void ) const {
		return _triangles;
	}

	void	Mesh::colors( const Colors& colors ) {
		_colors = colors; _gl.dirtyBufferGL = true;
	}
	const Mesh::Colors& Mesh::colors( void ) const {
		return _colors;
	}
	bool	Mesh::hasColors( void ) const {
		return (_vertices.size() > 0 && _vertices.size() == _colors.size());
	}

	void	Mesh::normals( const Normals& normals ) {
		_normals = normals; _gl.dirtyBufferGL = true;
	}
	const Mesh::Normals& Mesh::normals( void ) const {
		return _normals;
	}
	bool	Mesh::hasNormals( void ) const {
		return (_vertices.size() > 0 && _vertices.size() == _normals.size());
	}

	// IMPORTANT NOTE:
	// The safety of the following functions depends on the implementation
	// of vec struct used in Vertices/Colors/Normals/Triangles. If this vec struct
	// only contains 3 float, the following function will work. (Take care of
	// compiler that add some padding in classes).
	//std::vector<float> Mesh::vertexArray( void ) const {
	//	return std::vector<float>(&(_vertices[0][0]), &(_vertices[0][0]) + _vertices.size()*3);
	//}
	//std::vector<uint> Mesh::triangleArray( void ) const {
	//	return std::vector<uint>(&(_triangles[0][0]), &(_triangles[0][0]) + _triangles.size()*3);
	//}
	//std::vector<float> Mesh::colorArray( void ) const {
	//	return std::vector<float>(&(_colors[0][0]), &(_colors[0][0]) + _colors.size()*3);
	//}
	//std::vector<float> Mesh::normalArray( void ) const {
	//	return std::vector<float>(&(_normals[0][0]), &(_normals[0][0]) + _normals.size()*3);
	//}

	//*/

	const float* Mesh::vertexArray( void ) const {
		return _vertices.empty()? nullptr : &(_vertices[0][0]);
		//return std::vector<float>(&(_vertices[0][0]), &(_vertices[0][0]) + _vertices.size()*3);
	}
	const uint* Mesh::triangleArray( void ) const {
		return _triangles.empty() ? nullptr : &(_triangles[0][0]);
		//return std::vector<uint>(&(_triangles[0][0]), &(_triangles[0][0]) + _triangles.size()*3);
	}
	const float* Mesh::colorArray( void ) const {
		return _colors.empty() ? nullptr : &(_colors[0][0]);
		//return std::vector<float>(&(_colors[0][0]), &(_colors[0][0]) + _colors.size()*3);
	}
	const float* Mesh::normalArray( void ) const {
		return _normals.empty() ? nullptr : &(_normals[0][0]);
		//return std::vector<float>(&(_normals[0][0]), &(_normals[0][0]) + _normals.size()*3);
	}

	void	Mesh::texCoords( const UVs& texcoords ) {
		_texcoords = texcoords; _gl.dirtyBufferGL = true;
	}

	const Mesh::UVs& Mesh::texCoords( void ) const {
		return _texcoords;
	}

	bool	Mesh::hasTexCoords( void ) const {
		return (_vertices.size() > 0 && _vertices.size() == _texcoords.size());
		//return !_texcoords.empty();
	}

	const float* Mesh::texCoordArray( void ) const {
		return _texcoords.empty() ? nullptr : &(_texcoords[0][0]);
	}

	struct RenderingOptions {
		bool depthTest = true;
		bool backFaceCulling = true;
		Mesh::RenderMode mode = Mesh::FillRenderMode;
		bool frontFaceCulling = false;
		bool invertDepthTest = false;
	};

	//*/

} // namespace sibr

#endif // __SIBR_MESH_HPP__
