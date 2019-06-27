#pragma once

#include <core/graphics/Mesh.hpp>
#include <core/graphics/Shader.hpp>
#include <core/view/ViewBase.hpp>
#include <core/view/InteractiveCameraHandler.hpp>
#include <core/raycaster/CameraRaycaster.hpp>

#include <list>

namespace sibr {

	class MultiMeshManager;
	class MeshData;

	//hierarchy of shader wrappers, so there is no duplication for uniforms, init(), set() and render()

	class SIBR_VIEW_EXPORT ShaderAlphaMVP {
		SIBR_CLASS_PTR(ShaderAlphaMVP);
	public:
		virtual void initShader(const std::string & name, const std::string & vert, const std::string & frag, const std::string & geom = "");
		virtual void setUniforms(const Camera & eye, const MeshData & data);
		virtual void render(const Camera & eye, const MeshData & data);

	protected:
		GLShader				shader;
		GLuniform<Matrix4f>		mvp;
		GLuniform<float>		alpha = 1.0;
	};

	class SIBR_VIEW_EXPORT ColorMeshShader : public ShaderAlphaMVP {
	public:
		virtual void initShader(const std::string & name, const std::string & vert, const std::string & frag, const std::string & geom = "");
		virtual void setUniforms(const Camera & eye, const MeshData & data);

	protected:
		GLuniform<Vector3f>	user_color;
	};

	class SIBR_VIEW_EXPORT PointShader : public ColorMeshShader {
	public:
		void initShader(const std::string & name, const std::string & vert, const std::string & frag, const std::string & geom = "") override;
		virtual void setUniforms(const Camera & eye, const MeshData & data) override;
		virtual void render(const Camera & eye, const MeshData & data) override;
	protected:
		GLuniform<int> radius;
	};

	class SIBR_VIEW_EXPORT MeshShadingShader : public ColorMeshShader {
	public:
		void initShader(const std::string & name, const std::string & vert, const std::string & frag, const std::string & geom = "") override;
		virtual void setUniforms(const Camera & eye, const MeshData & data) override;

	protected:
		GLuniform<Vector3f>		light_position;
		GLuniform<bool>			phong_shading, use_mesh_color;
	};

	class SIBR_VIEW_EXPORT NormalRenderingShader : public ColorMeshShader {
	public:
		void initShader(const std::string & name, const std::string & vert, const std::string & frag, const std::string & geom = "") override;
		virtual void setUniforms(const Camera & eye, const MeshData & data) override;

	protected:
		GLuniform<float> normals_size;
	};


	//helper class containing all information relative to how to render a mesh for debugging purpose
	class SIBR_VIEW_EXPORT MeshData {
		SIBR_CLASS_PTR(MeshData);

	public:
		friend class MultiMeshManager;
		enum ColorMode : int { USER_DEFINED, VERTEX }; 
		enum MeshType : int { POINTS = 0, LINES = 1, TRIANGLES = 2, DUMMY };

		MeshData(const std::string & _name = "", Mesh::Ptr mesh_ptr = {}, MeshType mesh_type = TRIANGLES, Mesh::RenderMode render_mode = Mesh::FillRenderMode);

		void	renderGeometry() const;
		void	onGUI(const std::string & name);

		operator bool() const;
		std::string getInfos() const;

		MeshData & setColor(const Vector3f & col);
		MeshData & setBackFace(bool bf);
		MeshData & setDepthTest(bool dt);	
		MeshData & setColorRandom();
		MeshData & setRadiusPoint(int rad);
		MeshData & setAlpha(float alpha);
		MeshData & setColorMode(ColorMode mode);

		std::string			name;

		Mesh::Ptr			meshPtr;
		MeshType			meshType;
		Mesh::RenderMode	renderMode;

		Matrix4f			transformation = Matrix4f::Identity();

		Raycaster::Ptr		raycaster;

		bool				depthTest = true;
		bool				backFaceCulling = true;
		bool				frontFaceCulling = false;
		bool				invertDepthTest = false;
		bool				active = true;
		bool				phongShading = false;

		//points
		int					radius = 5;

		//colors
		ColorMode			colorMode = USER_DEFINED;
		Vector3f			userColor = { 0.5,0.5,0.5 };
		float				alpha = 1.0f;

		//normals
		enum NormalMode { PER_TRIANGLE, PER_VERTEX };
		MeshData getNormalsMeshData() const;

		Vector3f normalsColor = { 1,0,1 };
		float normalsLength = 1.0f;
		NormalMode normalMode = PER_TRIANGLE;
		bool normalsInverted = false;
		bool showNormals = false;

	protected:
		
		static MeshData dummy;
	};


	// debugging API to view/render and interract with several meshes
	class SIBR_VIEW_EXPORT MultiMeshManager : public ViewBase {
		SIBR_CLASS_PTR(MultiMeshManager);

	public:
		//the ctor will init shaders so require an OpenGL context
		MultiMeshManager(const std::string & _name = "MultiMeshManager");
	
		//multiple function to add a mesh to the manager
		// if no collision, returns a ref to the newly created meshData 
		// if collision, only the sibr::Mesh::Ptr is replaced, all options remains, and returns a ref to MeshData::dummy
		MeshData & addMesh(const std::string & name, Mesh::Ptr mesh, bool use_raycaster = true);
		//overload with user specified raycaster
		MeshData & addMesh(const std::string & name, Mesh::Ptr mesh, Raycaster::Ptr raycaster, bool create_raycaster = false);

		MeshData & addMeshAsLines(const std::string & name, Mesh::Ptr mesh);
		MeshData & addLines(const std::string & name, const std::vector<Vector3f> & endPoints, const Vector3f & color = { 0,1,0 });
		MeshData & addPoints(const std::string & name, const std::vector<Vector3f> & points, const Vector3f & color = { 1,0,0 });

		//accessor to previously added mesh, will return a reference to MeshData::dummy if no match
		MeshData & getMeshData(const std::string & name);
		
		void		removeMesh(const std::string & name);

		//ViewBase interface
		virtual void	onUpdate(Input& input, const Viewport & vp) override;
		virtual void	onRender(const Viewport & viewport) override;
		virtual void	onRender(IRenderTarget & dst);
		virtual void	onGUI() override;

		InteractiveCameraHandler & getCameraHandler() { return camera_handler; }
		MeshShadingShader & getMeshShadingShader() { return colored_mesh_shader; }

	protected:

		MeshData & addMeshData(MeshData & data, bool update_raycaster = false);
		void initShaders();
		void renderMeshes();
		void list_mesh_onGUI();

		using ListMesh = std::list<MeshData>;
		using Iterator = ListMesh::iterator;
	
		std::string							name;

		ListMesh							list_meshes;
		Iterator 							selected_mesh_it;
		bool								selected_mesh_it_is_valid = false;
		
		InteractiveCameraHandler			camera_handler;

		PointShader							points_shader;
		MeshShadingShader					colored_mesh_shader;
		NormalRenderingShader				per_vertex_normals_shader, per_triangle_normals_shader;

		Vector3f							backgroundColor = { 0.7f, 0.7f, 0.7f };
	};

}