#pragma once


#include <core/view/BasicIBRScene.hpp>
#include <core/assets/InputCamera.hpp>
#include <core/graphics/Texture.hpp>

#include <core/graphics/RenderUtility.hpp>
#include <core/view/Resources.hpp>
# include <core/view/ViewBase.hpp>

#include <core/view/MultiMeshManager.hpp>

#include <map>


//TEMPORARY

//# include "ViewUtils.hpp"

namespace sibr {
	
	//cam self near and far are used if parameters are < 0
	sibr::Mesh::Ptr generateCamFrustum(const InputCamera & cam, float znear = -1, float zfar = -1);

	sibr::Mesh::Ptr generateCamFrustumColored(const InputCamera & cam, const Vector3f & col, float znear = -1, float zfar = -1);

	sibr::Mesh::Ptr generateCamQuadWithUvs(const sibr::InputCamera & cam, float dist);

	class SIBR_VIEW_EXPORT ShaderImageArraySlice : public ShaderAlphaMVP {
	public:
		virtual void initShader(const std::string & name, const std::string & vert, const std::string & frag);
		virtual void render(const sibr::Camera & eye, const MeshData & data) {}
		void render(const sibr::Camera & eye, const MeshData & data, GLuint textureArrayId, int image_id);
	protected:
		sibr::GLuniform<int> slice = 1;
	};

	class SIBR_VIEW_EXPORT ShaderImageSlice : public ShaderAlphaMVP {
	public:
		virtual void render(const sibr::Camera & eye, const MeshData & data) {}
		void render(const sibr::Camera & eye, const MeshData & data, GLuint textureId);
	};

	class SIBR_VIEW_EXPORT TopView : public MultiMeshManager
	{
		SIBR_CLASS_PTR(TopView);

	public:
		TopView(BasicIBRScene::Ptr scene);

		virtual void	onRender(const sibr::Viewport & viewport) override;

		virtual void	onGUI() override;

		void changeScene(BasicIBRScene::Ptr scene);

		void updateActiveCams(const std::vector<int> & cams_id);

	protected:
		void setupMeshes();
		void initImageCamShaders();

		struct LabelMesh {
			Mesh::Ptr mesh;
			unsigned int splitIndex = 0;
		};

		/// Structure used to store information common to bothe scene and streamer but in different layouts.
		struct CameraInfos {
			CameraInfos(const sibr::InputCamera& cam, uint id, bool highlight);

			sibr::InputCamera cam;
			uint id = 0;
			bool highlight = false;
		};

		ShaderImageArraySlice	 _cameraImageShaderArray;
		ShaderImageSlice		 _cameraImageShader;

		std::map<unsigned int, LabelMesh> 		_labelMeshes;
		sibr::GLShader							_labelShader;
		sibr::GLuniform<sibr::Vector3f>			_labelShaderPosition;
		sibr::GLuniform<float>					_labelShaderScale;
		sibr::GLuniform<sibr::Vector2f>			_labelShaderViewport;
		float									_labelScale = 1.0f;
		bool									_showLabels = false;

		BasicIBRScene::Ptr _scene;

		std::vector<CameraInfos> _cameras;

		int					_snapToImage = 0;
		float				_alphaImage = 0.5f;
		float				_cameraScaling = 0.8f;
		bool				_showImages = true;
	};

}