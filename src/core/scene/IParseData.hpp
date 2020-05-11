#pragma once
#include "core/system/picojson/picojson.hpp"
#include "core/scene/Config.hpp"
#include "core/system/CommandLineArgs.hpp"
#
#include "core/system/Matrix.hpp"
#include "core/assets/ImageListFile.hpp"
#include "core/assets/InputCamera.hpp"


#include <iostream>
#include <vector>
#include <string>


namespace sibr{

	/**
	* Interface used to store the data required for defining an IBR Scene
	* 
	*
	* Members:
	* - _basePathName: Base dataset directory path.
	* - _camInfos: Vector of sibr::InputCamera holding all data attached with the scene cameras.
	* - _meshPath: Filepath of the mesh associated to the scene.
	* - _imgInfos: Vector of sibr::ImageListFile::Infos holding filename, width, height, and id of the input images.
	* - _imgPath: Path to the calibrated images directory.
	* - _activeImages: Vector of bools storing active state of the camera.
	* - _numCameras: Number of cameras associated with the dataset
	* - _datasetType: Type if dataset being used. Currently supported: COLMAP, SIBR_BUNDLER, NVM, MESHROOM
	*
	* \ingroup sibr_scene
	*/

	class SIBR_SCENE_EXPORT IParseData {
		
	public:

		/**
		 * \brief Denotes the type of dataset represented by a IParseData object.
		* \ingroup sibr_scene
		*/
		enum class Type {
			EMPTY, SIBR, COLMAP, NVM, MESHROOM, EXTERNAL
		};

		/**
		* \brief Pointer to the instance of class sibr::IParseData.
		*/
		typedef std::shared_ptr<IParseData>				Ptr;

		/**
		* \brief Function to parse data from a template dataset path.
		* \param dataset_path Path to the folder containing data
		* \param customPath Path to algorithm specific data
		* \param scene_metadata_filename Specify the filename of the Scene Metadata file to load specific scene
		*/
		virtual void  getParsedBundlerData(const std::string & dataset_path, const std::string & customPath, const std::string & scene_metadata_filename) = 0;

		/**
		* \brief Function to parse data from a template dataset path.
		* \param dataset_path Path to the folder containing data
		* \param customPath Path to algorithm specific data
		*/
		virtual void  getParsedMeshroomData(const std::string & dataset_path, const std::string & customPath = "") = 0;


		/**
		* \brief Function to parse data from a colmap sparse dataset path.
		* \param dataset_path Path to the colmap dataset sparse folder containing data
		*
		* The function takes in a colmap dataset sparse folder path and populates IParseData members with data.
		* This function can be used for direct compatibility with colmap data in SIBR.
		* The function automatically computes the intrinsic and extrinsic parameters of the camera, input images filename, widht and height etc.
		* Colmap uses LHS coordinate system while SIBR uses RHS coordinate system. The function applies appropriate transformation to handle this case.
		* 
		* For further compatibility with FrIBR, which enforces a Y-up RHS coordinate system, we need to apply an extra conversion to the rotation matrix, to 'flip back' from y-down to y-up.
		* \note Note: when applying the above mentioned conversion, the mesh needs to be converted by the same converter matrix
		* \brief Function to parse data from a colmap dataset path.
		* \param dataset_path Path to the folder containing data
		*/
		virtual void  getParsedColmapData(const std::string & dataset_path) = 0;

		/**
		* \brief Function to parse data from a template dataset path.
		* \param dataset_path Path to the folder containing data
		* \param customPath Path to algorithm specific data
		* \param nvm_path Specify the filename of the NVM path.
		*/
		virtual void  getParsedNVMData(const std::string & dataset_path, const std::string & customPath, const std::string & nvm_path) = 0;

		/**
		* \brief Function to parse data from a dataset path. Will automatically determine the type of dataset based on the files present.
		* \param myArgs Arguments containing the dataset path and other infos
		* \param customPath additional data path
		*/
		virtual void  getParsedData(const BasicIBRAppArgs & myArgs, const std::string & customPath = "") = 0;

		/**
		* \brief Getter for the information regarding the input images.
		*
		*/
		virtual const std::vector<sibr::ImageListFile::Infos>&	imgInfos(void) const = 0;

		/**
		* \brief Setter for the information regarding the input images.
		*
		*/
		virtual void											imgInfos(std::vector<sibr::ImageListFile::Infos>& infos) = 0;

		/**
		* \brief Getter to the number of cameras defined in the bundle file.
		*
		*/
		virtual const int										numCameras(void) const = 0;

		/**
		* \brief Setter to the number of cameras defined in the bundle file.
		*
		*/
		virtual void											numCameras(int numCams) = 0;

		/**
		* \brief Getter for the list of active cameras/images.
		*
		*/
		virtual const std::vector<bool>&						activeImages(void) const = 0;

		/**
		* \brief Setter for the list of active cameras/images.
		*
		*/
		virtual void											activeImages(std::vector<bool>& activeCams) = 0;

		/**
		* \brief Getter for the base path name where the dataset is located.
		*
		*/
		virtual const std::string&								basePathName(void) const = 0;

		/**
		* \brief Setter for the base path name where the dataset is located.
		*
		*/
		virtual void											basePathName(std::string & path)  = 0;
		
		/**
		* \brief Getter for the mesh path where the dataset is located.
		*
		*/
		virtual const std::string&								meshPath(void) const = 0;

		/**
		* \brief Setter for the mesh path where the dataset is located.
		*
		*/
		virtual void											meshPath(std::string & path)  = 0;

		/**
		* \brief Getter for the dataset type.
		*
		*/
		virtual const IParseData::Type&							datasetType(void) const = 0;

		/**
		* \brief Setter for the dataset type.
		*
		*/
		virtual void											datasetType(IParseData::Type dataType) = 0;

		/**
		* \brief Getter for the camera infos.
		*
		*/
		virtual const std::vector<InputCamera::Ptr>	cameras(void) const = 0;

		/**
		* \brief Setter for the camera infos.
		*
		*/
		virtual void											cameras(std::vector<InputCamera::Ptr>& cams) = 0;

		/**
		* \brief Getter for the image path.
		*
		*/
		virtual const std::string								imgPath(void) const = 0;

		/**
		* \brief Setter for the image path.
		*
		*/
		virtual void											imgPath(std::string& imPath) = 0;

		/**
		* \brief Function to parse the scene metadata file to read image data.
		*
		*/
		virtual bool											parseSceneMetadata(const std::string & scene_metadata_path) = 0;
		
	};

}