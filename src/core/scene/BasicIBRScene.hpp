#pragma once

#include <core/system/CommandLineArgs.hpp>
#include "core/raycaster/CameraRaycaster.hpp"
#include "core/scene/RenderTargetTextures.hpp"
#include "core/scene/Config.hpp"
#include "core/system/String.hpp"

namespace sibr {

	/**
	* Class used to define a basic IBR Scene 
	* containing multiple components required to define a scene.
	*
	* Members:
	* - CalibratedCameras
	* - InputImages
	* - ProxyMesh
	* - RenderTargetTextures
	* 
	* \ingroup sibr_scene
	*/
	class SIBR_SCENE_EXPORT BasicIBRScene
	{
		
	public:

		/**
		* \brief Pointer to the instance of class sibr::BasicIBRScene.
		*/
		SIBR_CLASS_PTR(BasicIBRScene);

		/**
		 * \brief Default constructor to create a BasicIBRScene.
		 */
		BasicIBRScene();

		/**
		 * \brief Constructor to create a BasicIBRScene given command line arguments.
		 * The scene may be created using either dataset path, or explicitly specifying individual componenets.
		 * \param myArgs to provide all command line arguments containing path to specific components.
		 * \param noRTs to specify whether to initialize render target textures or not.
		 * \param noMesh skip loading the mesh
		 */
		BasicIBRScene(const BasicIBRAppArgs & myArgs, bool noRTs = false, bool noMesh = false);

		/** Detructor. */
		~BasicIBRScene() {};

		/**
		* \brief Creates a BasicIBRScene given custom data argument.
		* The scene will be created using the custom data (cameras/images/proxies/textures etc.) provided.
		* \param data to provide data instance holding customized components.
		* \param noRTs to specify whether to initialize render target textures or not.
		* \param width the constrained width for GPU texture data.
		*/
		void createFromCustomData(const ParseData::Ptr & data, bool noRTs = false, const uint width = 0);

		/**
		* \brief Creates a BasicIBRScene from the internal stored data component in the scene.
		* The data could be populated either from dataset path or customized by the user externally.
		* \param noRTs to specify whether to initialize render target textures or not.
		* \param width the constrained width for GPU texture data.
		* \param noMesh skip loading the mesh
		*/
		void createFromData(bool noRTs = false, const uint width = 0, bool noMesh = false);

		/**
		 * \brief Function to create a scene directly using the dataset path specified in command-line.
		 */
		void	createFromDatasetPath();

		/**
		* \brief Function to generate render targets using the _data (regarding cameras, images, proxies ) parsed from metadata file.
		*/
		void	createRenderTargets();


		/**
		 * \brief Getter for the pointer holding the data related to the scene.
		 * 
		 */
		const ParseData::Ptr						data(void) const;

		/**
		* \brief Setter for the pointer holding the data related to the scene for scene creation.
		* \param data the setup data
		*/
		void									data(const sibr::ParseData::Ptr & data);


		/**
		 * \brief Getter for the pointer holding cameras related to each input iamge of the scene.
		 *
		 */
		const CalibratedCameras::Ptr				cameras(void) const;

		/**
		 * \brief Getter for the pointer holding the input images to the scene.
		 *
		 */
		const InputImages::Ptr						images(void) const;

		/**
		 * \brief Getter for the pointer holding the proxies required by the scene.
		 *
		 */
		const ProxyMesh::Ptr						proxies(void) const;

		/**
		 * \brief Getter for the pointer holding the render targets textures related to the scene.
		 *
		 */
		const RenderTargetTextures::Ptr	&			renderTargets(void) const;
		
		/**
		 * \brief Getter for the pointer holding the render targets textures related to the scene.
		 *
		 */
		RenderTargetTextures::Ptr &			renderTargets(void);

		/**
		 * \brief Getter for the pointer holding the mesh textures related to the mesh loaded for the scene.
		 *
		 */
		Texture2DRGB::Ptr &					inputMeshTextures(void);
		

	protected:
		BasicIBRScene(BasicIBRScene & scene);
		BasicIBRScene& operator =(const BasicIBRScene&) = delete;

		ParseData::Ptr								_data;
		CalibratedCameras::Ptr						_cams;
		InputImages::Ptr							_imgs;
		ProxyMesh::Ptr								_proxies;
		Texture2DRGB::Ptr							_inputMeshTexture;
		RenderTargetTextures::Ptr					_renderTargets;
		InputCamera									_userCamera;
		
	};

	///// INLINE DEFINITIONS /////

	inline const ParseData::Ptr			BasicIBRScene::data(void) const
	{
		return _data;
	}

	inline void BasicIBRScene::data(const sibr::ParseData::Ptr & data) 
	{
		_data = data;
	}

	inline const CalibratedCameras::Ptr BasicIBRScene::cameras(void) const
	{
		return _cams;
	}

	inline const InputImages::Ptr BasicIBRScene::images(void) const
	{
		return _imgs;
	}

	inline const ProxyMesh::Ptr BasicIBRScene::proxies(void) const
	{
		return _proxies;
	}

	inline const RenderTargetTextures::Ptr & BasicIBRScene::renderTargets(void) const
	{
		return _renderTargets;
	}

	inline RenderTargetTextures::Ptr & BasicIBRScene::renderTargets(void)
	{
		return _renderTargets;
	}

	inline Texture2DRGB::Ptr & BasicIBRScene::inputMeshTextures(void)
	{
		return _inputMeshTexture;
	}

}
