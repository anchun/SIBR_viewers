#pragma once

#include <core/scene/IIBRScene.hpp>

namespace sibr {

	/**
	* Class used to define a basic IBR Scene 
	* containing multiple components required to define a scene.
	* 
	* \ingroup sibr_scene
	*/
	class SIBR_SCENE_EXPORT BasicIBRScene: public IIBRScene
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
		BasicIBRScene(const BasicIBRAppArgs & myArgs, bool noRTs, bool noMesh = false);

		/**
		 * \brief Constructor to create a BasicIBRScene given command line arguments.
		 * The scene may be created using either dataset path, or explicitly specifying individual componenets.
		 * \param myArgs to provide all command line arguments containing path to specific components.
		 * \param myOpts to specify initialization paramters for the scene.
		 */
		BasicIBRScene(const BasicIBRAppArgs& myArgs, SceneOptions myOpts = SceneOptions());


		/** Destructor. */
		~BasicIBRScene() {};

		/**
		* \brief Creates a BasicIBRScene given custom data argument.
		* The scene will be created using the custom data (cameras/images/proxies/textures etc.) provided.
		* \param data to provide data instance holding customized components.
		* \param width the constrained width for GPU texture data.
		* \param myOpts to specify whether to initialize specific parts of the scene (RTs, geometry,...)
		*/
		void createFromCustomData(const IParseData::Ptr & data, const uint width = 0, SceneOptions myOpts = SceneOptions()) override;
		
		/**
		 * \brief Function to create a scene directly using the dataset path specified in command-line.
		 */
		void createFromDatasetPath() {};

		/**
		* \brief Function to generate render targets using the _data (regarding cameras, images, proxies ) parsed from metadata file.
		*/
		void createRenderTargets() override;
		

	protected:
		BasicIBRScene(BasicIBRScene & scene);
		BasicIBRScene& operator =(const BasicIBRScene&) = delete;

		SceneOptions				_currentOpts;

		/**
		* \brief Creates a BasicIBRScene from the internal stored data component in the scene.
		* The data could be populated either from dataset path or customized by the user externally.
		* \param width the constrained width for GPU texture data.
		*/
		void createFromData(const uint width = 0);

		
	};

}
