
#ifndef __SIBR_MATERIALMESH_HPP__
# define __SIBR_MATERIALMESH_HPP__

# include <vector>
# include <map>
# include <sstream>

# include "core/graphics/Config.hpp"
# include "core/system/Vector.hpp"
# include "core/graphics/Image.hpp"
# include "core/graphics/Mesh.hpp"
# include "core/graphics/MeshBufferGL.hpp"
# include "core/graphics/Texture.hpp"

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
	class SIBR_GRAPHICS_EXPORT MaterialMesh : public sibr::Mesh
	{
	public:
		typedef std::vector<int>								MatIds;
		typedef std::vector<std::string>						MatId2Name;


		typedef std::map<std::string, sibr::ImageRGB::Ptr>		OpacityMaps;
		typedef std::map<std::string, sibr::ImageRGB::Ptr>		DiffuseMaps;

		typedef std::vector<Mesh>								SubMeshes;
		typedef std::vector<sibr::ImageRGB>						AlbedoTextures;

		SIBR_CLASS_PTR(MaterialMesh);

		enum class RenderCategory
		{
			classic,
			diffuseMaterials,
			threesixtyMaterials,
			threesixtyDepth
		};

		struct AmbientOcclusion {
			bool AoIsActive = false;
			float AttenuationDistance = 1.f;
			float IlluminanceCoefficient = 1.f;
			float SubdivideThreshold = 10.f;
		};
		typedef struct AmbientOcclusion AmbientOcclusion;

		std::string vertexShaderAlbedo =
			"#version 450										\n"
			"layout(location = 0) in vec3 in_vertex;			\n"
			"layout(location = 1) in vec3 in_colors;			\n"
			"layout(location = 2) in vec2 in_uvCoords;			\n"
			"layout(location = 3) in vec3 in_normal;			\n"
			"layout(location = 4) in float in_ao;			\n"
			"//layout(location = 4) in float in_material;			\n"
			"layout (location = 2) out vec2 uvCoords;			\n"
			"//out float material;			\n"
			"layout (location = 3) out vec3 normal;									\n"
			"out float ao ;									\n"
			"layout (location = 1) out vec3 colors;									\n"
			"uniform mat4 MVP;									\n"
			"void main(void) {									\n"
			"	normal = in_normal;		\n"
			"	ao = in_ao;		\n"
			"	uvCoords = in_uvCoords;		\n"
			"	colors= in_colors;		\n"
			"	//material= float(in_material);		\n"
			"	gl_Position = MVP*vec4(in_vertex,1) ;		\n"
			"}													\n";

		std::string fragmentShaderAlbedo =
			"#version 450														\n"
			"layout(binding = 0) uniform sampler2D tex;				\n"
			"layout(binding = 2) uniform sampler2D opacity;				\n"
			"uniform int layer;													\n"
			"uniform bool AoIsActive;													\n"
			"uniform vec2 grid;												\n"
			"uniform float IlluminanceCoefficient;												\n"
			"layout (location = 2) in vec2 uvCoords;													\n"
			"layout (location = 3) in vec3 normal ;									\n"
			"layout (location = 1) in vec3 colors;									\n"
			"out vec4 out_color;												\n"
			"void main(void) {													\n"
			"	vec4 opacityColor;												\n"
			"	vec3 colorsModified = colors;\n"
			"	float lighter_ao = colors.x * IlluminanceCoefficient; \n"
			"	if (lighter_ao > 1.f ) lighter_ao = 1.f;\n"
			"	colorsModified.x = lighter_ao;\n"
			"	colorsModified.y = lighter_ao;\n"
			"	colorsModified.z = lighter_ao;\n"
			"	//int n = int (material);	 \n"
			"	//out_color = mix(out_color, texture(texArray,vec3(uvCoords.x,uvCoords.y,n)), 0.9);\n"
			"	//out_color = texture(texArray,vec3(uvCoords.x,1.0-uvCoords.y,n));\n"
			"	//out_color = vec4(normal,1.0);\n"
			"	opacityColor = texture(opacity,vec2(uvCoords.x,1.0-uvCoords.y));\n"
			"	if (opacityColor.x < 0.1f) discard;						\n"
			"	if (AoIsActive ) {						\n"
			"	out_color = texture(tex,vec2(uvCoords.x,1.0-uvCoords.y))* vec4(colorsModified,1);\n"
			"	out_color = vec4(out_color.x,out_color.y,out_color.z,opacityColor.x);\n}"
			"	else /*(colors.x == 0 && colors.y == 0 && colors.z == 0)*/	\n"
			"	out_color = texture(tex,vec2(uvCoords.x,1.0-uvCoords.y));\n"
			"	//out_color = vec4(colors,1);										\n"
			"}																	\n";
	public:

		MaterialMesh(bool withGraphics = true) : Mesh(withGraphics) {
		}
		MaterialMesh(sibr::Mesh& mesh) : Mesh(mesh) {}

		/// Set material Ids
		inline void	matIds(const MatIds& matIds);
		/// Get material Ids
		inline const MatIds& matIds(void) const;
		/// Get material vertices Ids 
		inline const MatIds& matIdsVertices(void) const;
		/// Return TRUE if each triangle has a materialId assigned
		inline bool	hasMatIds(void) const;
		/// Return the mapping between Ids and Material Name
		inline const MatId2Name& matId2Name(void) const;
		/// Set the mapping
		inline void matId2Name(const MatId2Name& matId2Name);

		/// Return the pointer to oppacity texture if it exist
		inline sibr::ImageRGB::Ptr opacityMap(const std::string& matName) const;
		/// Set the opacityMaps
		inline void opacityMaps(const OpacityMaps & maps);
		/// get the opacityMaps
		inline const OpacityMaps& opacityMaps(void) const;

		/// Return the pointer to oppacity texture if it exist
		inline sibr::ImageRGB::Ptr diffuseMap(const std::string& matName) const;
		/// Set the diffuseMaps
		inline void diffuseMaps(const DiffuseMaps & maps);
		/// get the diffuseMaps
		inline const DiffuseMaps& diffuseMaps(void) const;

		/// set the subMeshes 
		inline void subMeshes(const SubMeshes& subMeshes);
		/// get the subMeshes 
		inline const SubMeshes& subMeshes(void) const;

		/// set the category of render
		inline void typeOfRender(const RenderCategory& type);
		/// get the category of render 
		inline const RenderCategory& typeOfRender(void) const;

		/// set the option Ambient occlusion
		void ambientOcclusion(const AmbientOcclusion& ao);

		/// get the option Ambient occlusion
		inline const AmbientOcclusion& ambientOcclusion(void);

		inline void aoFunction(std::function<sibr::Mesh::Colors(
			sibr::MaterialMesh&,
			const int)>& aoFunction);

		/// Load a material mesh from the disk.
		/// (tested with .ply, should work with .obj)
		bool	load(const std::string& filename);

		/// Load a material mesh from a mitsuba XML files.
		/// It allows handling instances used several times.
		bool	loadMtsXML(const std::string& filename);

		void	fillColorsWithIndexMaterials(void);


		sibr::Mesh::Colors genAO(const int spp);

		/// Merge another material_mesh into this one
		void	merge(const MaterialMesh& other);

		/// Make the mesh whole. ie: it will have default values for all components (texture, materials, colors, etc)
		/// It is usefull when merging two meshes. If the second one is missing some attributes, the merging will break the good mesh.
		void	makeWhole(void);

		void	createSubMeshes(void);


		sibr::MaterialMesh::Ptr invertedFacesMesh2() const;

		void	forceBufferGLUpdate(void) const;
		void	freeBufferGLUpdate(void) const;

		void subdivideMesh2(float threshold);

		void	subdivideMesh(float threshold);


		void addEnvironmentMap(float* forcedCenterX = nullptr,
			float* forcedCenterY = nullptr,
			float* forcedCenterZ = nullptr,
			float* forcedRadius = nullptr);


		void	render(
			bool depthTest = true,
			bool backFaceCulling = true,
			RenderMode mode = FillRenderMode,
			bool frontFaceCulling = false,
			bool invertDepthTest = false,
			bool tessellation = false
		) const;

		void	renderAlbedo(
			bool depthTest = true,
			bool backFaceCulling = true,
			RenderMode mode = FillRenderMode,
			bool frontFaceCulling = false,
			bool invertDepthTest = false
		) const;

		void	renderThreeSixty(
			bool depthTest,
			bool backFaceCulling,
			RenderMode mode,
			bool frontFaceCulling,
			bool invertDepthTest
		) const;

		void	initAlbedoTextures(void);
		Mesh generateSubMaterialMesh(int material) const;

	private:


		MatIds		_matIds;
		MatIds		_matIdsVertices;
		MatId2Name	_matId2Name;
		OpacityMaps _opacityMaps;
		DiffuseMaps _diffuseMaps;

		// We have to gen one mesh per material to render them separately
		SubMeshes	_subMeshes;
		RenderCategory _typeOfRender = RenderCategory::diffuseMaterials;

		bool _albedoTexturesInitialized = false;
		std::vector<sibr::Texture2DRGB::Ptr> _albedoTextures;
		std::vector<GLuint> _idTextures;

		std::vector<sibr::Texture2DRGB::Ptr> _opacityTextures;
		std::vector<GLuint> _idTexturesOpacity;

		//AO attributes
		AmbientOcclusion _ambientOcclusion;
		std::function<sibr::Mesh::Colors(sibr::MaterialMesh&, const int)> _aoFunction;
		float _currentThreshold;
		bool _aoInitialized = false;
		float _averageSize;
		float _averageArea;
	};

	///// DEFINITION /////



	void	MaterialMesh::matIds(const MatIds& matIds) {
		_matIds = matIds;
	}
	const MaterialMesh::MatIds& MaterialMesh::matIds(void) const {
		return _matIds;
	}
	bool	MaterialMesh::hasMatIds(void) const {
		return (_triangles.size() > 0 && _triangles.size() == _matIds.size());
	}
	const MaterialMesh::MatIds& MaterialMesh::matIdsVertices(void) const {
		return _matIdsVertices;
	}
	const MaterialMesh::MatId2Name& MaterialMesh::matId2Name(void) const {
		return _matId2Name;
	}
	void MaterialMesh::matId2Name(const MatId2Name & matId2Name)
	{
		_matId2Name = matId2Name;
	}

	// Opacity map function
	ImageRGB::Ptr MaterialMesh::opacityMap(const std::string& matName) const
	{
		auto & el = _opacityMaps.find(matName);
		if (el != _opacityMaps.end()) {
			return el->second;
		}
		return nullptr;
	}
	const MaterialMesh::OpacityMaps& MaterialMesh::opacityMaps(void) const
	{
		return _opacityMaps;
	}

	void MaterialMesh::opacityMaps(const OpacityMaps& maps)
	{
		_opacityMaps = maps;
	}

	ImageRGB::Ptr MaterialMesh::diffuseMap(const std::string& matName) const
	{
		auto & el = _diffuseMaps.find(matName);
		if (el != _diffuseMaps.end()) {
			return el->second;
		}
		return nullptr;
	}

	/*ImageRGB MaterialMesh::diffuseMap(const std::string& matName)
	{
		auto & el =_diffuseMaps.find(matName);
		if (el != _diffuseMaps.end()) {
			return el->second;
		}
	}*/

	const MaterialMesh::DiffuseMaps& MaterialMesh::diffuseMaps(void) const
	{
		return _diffuseMaps;
	}

	void MaterialMesh::diffuseMaps(const DiffuseMaps& maps)
	{
		_diffuseMaps = maps;
	}

	const MaterialMesh::SubMeshes& MaterialMesh::subMeshes(void) const
	{
		return _subMeshes;
	}


	void MaterialMesh::subMeshes(const SubMeshes& subMeshes)
	{
		_subMeshes = subMeshes;
	}

	const MaterialMesh::RenderCategory& MaterialMesh::typeOfRender(void) const {
		return _typeOfRender;
	}
	

	inline const MaterialMesh::AmbientOcclusion & MaterialMesh::ambientOcclusion(void)
	{
		return _ambientOcclusion;
	}

	inline void MaterialMesh::aoFunction(std::function<sibr::Mesh::Colors
	(sibr::MaterialMesh&, const int)>&
		aoFunction)
	{
		_aoFunction = aoFunction;
	}

	void MaterialMesh::typeOfRender(const RenderCategory& type) {
		_typeOfRender = type;
	}


} // namespace sibr

#endif // __SIBR_MATERIALMESH_HPP__
