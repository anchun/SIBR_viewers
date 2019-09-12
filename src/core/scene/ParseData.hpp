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
	* Class used to hold the data required for defining an IBR Scene
	* 
	*
	* Members:
	* - basePathName: Base dataset path name
	* - 
	* - 
	* - 
	*
	* \ingroup sibr_assets
	*/

	class SIBR_SCENE_EXPORT ParseData{
		
	public:

		/**
		 * \brief Denotes the type of dataset represented by a ParseData object.
		* \ingroup sibr_view
		*/
		enum class Type {
			EMPTY, SIBR, COLMAP, NVM, MESHROOM
		};

		/**
		* \brief Pointer to the instance of class sibr::ParseData.
		*/
		typedef std::shared_ptr<ParseData>				Ptr;

		/**
		* \brief Function to parse data from a template dataset path.
		* \param dataset_path Path to the folder containing data
		* \param customPath Path to algorithm specific data
		* \param scene_metadata_filename Specify the filename of the Scene Metadata file to load specific scene
		*/
		void  getParsedBundlerData(const std::string & dataset_path, const std::string & customPath, const std::string & scene_metadata_filename);

		/**
		* \brief Function to parse data from a template dataset path.
		* \param dataset_path Path to the folder containing data
		* \param customPath Path to algorithm specific data
		*/
		void  getParsedMeshroomData(const std::string & dataset_path, const std::string & customPath = "");


		/**
		* \brief Function to parse data from a colmap sparse dataset path.
		* \param dataset_path Path to the colmap dataset sparse folder containing data
		*
		* The function takes in a colmap dataset sparse folder path and populates ParseData members with data.
		* This function can be used for direct compatibility with colmap data in SIBR.
		* The function automatically computes the intrinsic and extrinsic parameters of the camera, input images filename, widht and height etc.
		* Colmap uses LHS coordinate system while SIBR uses RHS coordinate system. The function applies appropriate transformation to handle this case.
		* 
		* For further compatibility with FrIBR, which enforces a Y-up RHS coordinate system, we need to apply an extra conversion to the rotation matrix, to 'flip back' from y-down to y-up.
		* \note Note: when applying the above mentioned conversion, the mesh needs to be converted by the same converter matrix
		* \brief Function to parse data from a colmap dataset path.
		* \param dataset_path Path to the folder containing data
		*/
		void  getParsedColmapData(const std::string & dataset_path);

		/**
		* \brief Function to parse data from a template dataset path.
		* \param dataset_path Path to the folder containing data
		* \param customPath Path to algorithm specific data
		* \param scene_metadata_filename Specify the filename of the Scene Metadata file to load specific scene
		*/
		void  getParsedNVMData(const std::string & dataset_path, const std::string & customPath, const std::string & nvm_path);

		/**
		* \brief Function to parse data from a dataset path. Will automatically determine the type of dataset based on the files present.
		* \param myArgs Arguments containing the dataset path and other infos
		*/
		void  getParsedData(const BasicIBRAppArgs & myArgs, const std::string & customPath = "");

		/**
		* \brief Getter for the information regarding the input images.
		*
		*/
		const std::vector<sibr::ImageListFile::Infos>&	imgInfos(void) const;

		/**
		* \brief Getter to the list of camera matrices associated with each input camera.
		*
		*/
		const std::vector<Matrix4f>&					outputCamsMatrix(void) const;

		/**
		* \brief Getter to the number of cameras defined in the bundle file.
		*
		*/
		const int										numCameras(void) const;

		/**
		* \brief Getter to the list of near and far clipping plane defined for each camera.
		*
		*/
		const std::vector<InputCamera::Z>&				nearsFars(void) const;

		/**
		* \brief Getter for the list of active cameras/images.
		*
		*/
		const std::vector<bool>&						activeImages(void) const;

		/**
		* \brief Getter for the list of in-active cameras/images.
		*
		*/
		const std::vector<bool>&						excludeImages(void) const;

		/**
		* \brief Setter for the list of in-active cameras/images.
		*
		*/
		void										excludeImages(std::vector<bool> & excImg);

		/**
		* \brief Getter for the base path name where the dataset is located.
		*
		*/
		const std::string&								basePathName(void) const;
		
		/**
		* \brief Getter for the mesh path where the dataset is located.
		*
		*/
		const std::string&								meshPath(void) const;

		/**
		* \brief Getter for the mesh path where the dataset is located.
		*
		*/
		const ParseData::Type&								datasetType(void) const;



	public:
			/**
		* \brief Function to parse the scene metadata file to read image data.
		*
		*/
			bool parseSceneMetadata(const std::string & scene_metadata_path);

	protected:

		struct CameraParametersColmap {
			size_t id;
			size_t width;
			size_t height;
			float  fx;
			float  fy;
			float  dx;
			float  dy;
		};

		/**
		* \brief Function to parse the camera calibration files to read camera properties (camera matrix etc.).
		*
		*/
		bool parseBundlerFile(const std::string & bundler_file_path);

		/**
		* \brief Function to parse the colmap sparse data files to read camera and image data.
		*
		*/
		bool parseColmapSparseData(const std::string & scene_sparse_path);

		/**
		* \brief Function to parse the NVM data files to read camera and image data.
		*
		*/
		bool parseNVMData(const std::string & nvm_path);


		bool parseMeshroomData(const std::string & sfm_path);


		std::string									_basePathName;
		std::string									_meshPath;
		std::vector<sibr::ImageListFile::Infos>		_imgInfos;
		std::vector<bool>							_activeImages;
		std::vector<bool>							_excludeImages;
		std::vector<Matrix4f>						_outputCamsMatrix;
		int											_numCameras;
		std::vector<InputCamera::Z>					_nearsFars;
		Type										_datasetType = Type::EMPTY;
		
	};


	///// INLINE DEFINITIONS /////
	
	inline const std::vector<sibr::ImageListFile::Infos>&	ParseData::imgInfos(void) const {
		return _imgInfos;
	}
	
	inline const std::vector<Matrix4f>& ParseData::outputCamsMatrix(void) const {
		return _outputCamsMatrix;
	}

	inline const int ParseData::numCameras( void ) const {		return _numCameras;		}
	
	inline const std::vector<InputCamera::Z>& ParseData::nearsFars(void) const {
		return _nearsFars;
	}
	
	inline const std::vector<bool>& ParseData::activeImages(void) const {
		return _activeImages;
	}

	inline const std::vector<bool>& ParseData::excludeImages(void) const {
		return _excludeImages;
	}

	inline void ParseData::excludeImages(std::vector<bool> & excImg) {
		_excludeImages = excImg;
	}

	inline const std::string & ParseData::basePathName(void) const
	{
		return _basePathName;
	}

	inline const std::string & ParseData::meshPath(void) const
	{
		return _meshPath;
	}

	inline const ParseData::Type & ParseData::datasetType(void) const
	{
		return _datasetType;
	}
}