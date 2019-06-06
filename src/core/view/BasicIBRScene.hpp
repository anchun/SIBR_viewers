#pragma once
#include "core/view/Config.hpp"
#include <core/system/CommandLineArgs.hpp>
#include "core/view/RenderTargetTextures.hpp"

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
	* \ingroup sibr_view
	*/

	class SIBR_VIEW_EXPORT BasicIBRScene
	{
		SIBR_DISALLOW_COPY(BasicIBRScene);
	public:

		/**
		* \brief Pointer to the instance of class sibr::BasicIBRScene.
		*/
		typedef std::shared_ptr<BasicIBRScene>									Ptr;

	public:

		/**
		 * \brief Default constructor to create a BasicIBRScene.
		 */
		BasicIBRScene();

		/**
		 * \brief Constructor to create a BasicIBRScene given command line arguments.
		 * The scene may be created using either dataset path, or explicitly specifying individual componenets.
		 * \param myArgs to provide all command line arguments containing path to specific components.
		 * \param noRTs to specify whether to initialize render target textures or not.
		 */
		BasicIBRScene::BasicIBRScene(BasicIBRAppArgs & myArgs, bool noRTs = false);

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
		*/
		void createFromData(bool noRTs = false, const uint width = 0);

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
		*
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
		const RenderTargetTextures::Ptr				renderTargets(void) const;
		
		/**
		 * \brief Getter for the pointer holding the user camera viewing the scene.
		 * 
		 * \todo Move to somwhere more appropriate.
		 */
		const sibr::InputCamera&					userCamera(void);

		/**
		 * \brief Setter for the pointer holding the user camera viewing the scene.
		 * \param cam The camera to which the user camera should be set.
		 * \todo Move to somwhere more appropriate.
		 */
		void										userCamera(const InputCamera& cam);
		

	protected:
		ParseData::Ptr								_data;
		CalibratedCameras::Ptr						_cams;
		InputImages::Ptr							_imgs;
		ProxyMesh::Ptr								_proxies;
		RenderTargetTextures::Ptr					_renderTargets;
		sibr::InputCamera							_userCamera;
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

	inline const RenderTargetTextures::Ptr BasicIBRScene::renderTargets(void) const
	{
		return _renderTargets;
	}

	inline const sibr::InputCamera & BasicIBRScene::userCamera(void)
	{
		return _userCamera;
	}

	inline void BasicIBRScene::userCamera(const InputCamera & cam)
	{
		_userCamera = cam;
	}

}
