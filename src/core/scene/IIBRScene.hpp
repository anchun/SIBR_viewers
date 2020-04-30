#pragma once

#include <core/system/CommandLineArgs.hpp>
#include "core/raycaster/CameraRaycaster.hpp"
#include "core/scene/ICalibratedCameras.hpp"
#include "core/scene/IParseData.hpp"
#include "core/scene/IProxyMesh.hpp"
#include "core/scene/IInputImages.hpp"
#include "core/scene/RenderTargetTextures.hpp"
#include "core/scene/Config.hpp"
#include "core/system/String.hpp"

namespace sibr {

	/**
	* Interface used to define how an IBR Scene is shaped
	* containing multiple components required to define a scene.
	*
	* Members:
	* - ICalibratedCameras
	* - IInputImages
	* - IProxyMesh
	* - RenderTargetTextures
	* 
	* \ingroup sibr_scene
	*/
	class SIBR_SCENE_EXPORT IIBRScene
	{

		/**
		* \brief Pointer to the instance of class sibr::IIBRScene.
		*/
		SIBR_CLASS_PTR(IIBRScene);
		
	public:
		/** Scene initialization infos. */
		struct SceneOptions
		{
			bool		renderTargets = true; ///< Load rendertargets?
			bool		mesh = true; ///< Load mesh?
			bool		images = true; ///< Load images?
			bool		cameras = true; ///< Load cameras?
		};

		/**
		* \brief Creates a BasicIBRScene given custom data argument.
		* The scene will be created using the custom data (cameras/images/proxies/textures etc.) provided.
		* \param data to provide data instance holding customized components.
		* \param width the constrained width for GPU texture data.
		* \param myOpts to specify whether to initialize specific parts of the scene (RTs, geometry,...)
		*/
		virtual void createFromCustomData(const IParseData::Ptr & data, const uint width = 0, SceneOptions myOpts = SceneOptions()) = 0;
		
		/**
		 * \brief Function to create a scene directly using the dataset path specified in command-line.
		 */
		virtual void	createFromDatasetPath() = 0;

		/**
		* \brief Function to generate render targets using the _data (regarding cameras, images, proxies ) parsed from metadata file.
		*/
		virtual void	createRenderTargets() = 0;


		/**
		 * \brief Getter for the pointer holding the data related to the scene.
		 * 
		 */
		const IParseData::Ptr						data(void) const;

		/**
		* \brief Setter for the pointer holding the data related to the scene for scene creation.
		* \param data the setup data
		*/
		void										data(const sibr::IParseData::Ptr & data);


		/**
		 * \brief Getter for the pointer holding cameras related to each input iamge of the scene.
		 *
		 */
		const ICalibratedCameras::Ptr				cameras(void) const;

		/**
		 * \brief Getter for the pointer holding the input images to the scene.
		 *
		 */
		const IInputImages::Ptr						images(void) const;

		/**
		 * \brief Getter for the pointer holding the proxies required by the scene.
		 *
		 */
		const IProxyMesh::Ptr						proxies(void) const;

		/**
		 * \brief Getter for the pointer holding the render targets textures related to the scene.
		 *
		 */
		const RenderTargetTextures::Ptr	&		renderTargets(void) const;
		
		/**
		 * \brief Getter for the pointer holding the render targets textures related to the scene.
		 *
		 */
		RenderTargetTextures::Ptr &				renderTargets(void);

		/**
		 * \brief Getter for the pointer holding the mesh textures related to the mesh loaded for the scene.
		 *
		 */
		Texture2DRGB::Ptr &						inputMeshTextures(void);
		

	protected:

		IParseData::Ptr				_data;
		ICalibratedCameras::Ptr		_cams;
		IInputImages::Ptr			_imgs;
		IProxyMesh::Ptr				_proxies;
		Texture2DRGB::Ptr			_inputMeshTexture;
		RenderTargetTextures::Ptr	_renderTargets;

		/**
		* \brief Creates an IIBRScene from the internal stored data component in the scene.
		* The data could be populated either from dataset path or customized by the user externally.
		* \param width the constrained width for GPU texture data.
		*/
		virtual void createFromData(const uint width = 0) = 0;

		
	};

	///// INLINE DEFINITIONS /////

	inline const IParseData::Ptr			IIBRScene::data(void) const
	{
		return _data;
	}

	inline void IIBRScene::data(const IParseData::Ptr & data) 
	{
		_data = data;
	}

	inline const ICalibratedCameras::Ptr IIBRScene::cameras(void) const
	{
		return _cams;
	}

	inline const IInputImages::Ptr IIBRScene::images(void) const
	{
		return _imgs;
	}

	inline const IProxyMesh::Ptr IIBRScene::proxies(void) const
	{
		return _proxies;
	}

	inline const RenderTargetTextures::Ptr & IIBRScene::renderTargets(void) const
	{
		return _renderTargets;
	}

	inline RenderTargetTextures::Ptr & IIBRScene::renderTargets(void)
	{
		return _renderTargets;
	}

	inline Texture2DRGB::Ptr & IIBRScene::inputMeshTextures(void)
	{
		return _inputMeshTexture;
	}
}