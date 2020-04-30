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
		const std::vector<sibr::ImageListFile::Infos>&	imgInfos(void) const;

		/**
		* \brief Setter for the information regarding the input images.
		*
		*/
		void											imgInfos(std::vector<sibr::ImageListFile::Infos>& infos);

		/**
		* \brief Getter to the number of cameras defined in the bundle file.
		*
		*/
		const int										numCameras(void) const;

		/**
		* \brief Setter to the number of cameras defined in the bundle file.
		*
		*/
		void											numCameras(int numCams);

		/**
		* \brief Getter for the list of active cameras/images.
		*
		*/
		const std::vector<bool>&						activeImages(void) const;

		/**
		* \brief Setter for the list of active cameras/images.
		*
		*/
		void											activeImages(std::vector<bool>& activeCams);

		/**
		* \brief Getter for the base path name where the dataset is located.
		*
		*/
		const std::string&								basePathName(void) const;

		/**
		* \brief Setter for the base path name where the dataset is located.
		*
		*/
		void											basePathName(std::string & path) ;
		
		/**
		* \brief Getter for the mesh path where the dataset is located.
		*
		*/
		const std::string&								meshPath(void) const;

		/**
		* \brief Setter for the mesh path where the dataset is located.
		*
		*/
		void											meshPath(std::string & path) ;

		/**
		* \brief Getter for the dataset type.
		*
		*/
		const IParseData::Type&							datasetType(void) const;

		/**
		* \brief Setter for the dataset type.
		*
		*/
		void											datasetType(IParseData::Type dataType);

		/**
		* \brief Getter for the camera infos.
		*
		*/
		const std::vector<InputCamera::Ptr>			cameras(void) const;

		/**
		* \brief Setter for the camera infos.
		*
		*/
		void											cameras(std::vector<InputCamera::Ptr>& cams);

		/**
		* \brief Getter for the image path.
		*
		*/
		const std::string								imgPath(void) const;

		/**
		* \brief Setter for the image path.
		*
		*/
		void											imgPath(std::string& imPath);

		/**
		* \brief Function to parse the scene metadata file to read image data.
		*
		*/
		virtual bool									parseSceneMetadata(const std::string & scene_metadata_path) = 0;

	protected:

		std::vector<InputCamera::Ptr>	_camInfos;
		std::string									_basePathName;
		std::string									_meshPath;
		std::vector<sibr::ImageListFile::Infos>		_imgInfos;
		std::string									_imgPath = "";
		std::vector<bool>							_activeImages;
		int											_numCameras;		
		Type										_datasetType = Type::EMPTY;
		
	};


	///// INLINE DEFINITIONS /////
	
	inline const std::vector<sibr::ImageListFile::Infos>&	IParseData::imgInfos(void) const {
		return _imgInfos;
	}

	inline void IParseData::imgInfos(std::vector<sibr::ImageListFile::Infos>& infos)
	{
		_imgInfos = infos;
	}

	inline const int IParseData::numCameras( void ) const {		
		return _numCameras;		
	}

	inline void IParseData::numCameras(int numCams)
	{
		_numCameras = numCams;
	}
	
	inline const std::vector<bool>& IParseData::activeImages(void) const {
		return _activeImages;
	}

	inline void IParseData::activeImages(std::vector<bool>& activeCams)
	{
		_activeImages = activeCams;
	}

	inline const std::string & IParseData::basePathName(void) const
	{
		return _basePathName;
	}

	inline void IParseData::basePathName(std::string& path)
	{
		_basePathName = path;
	}

	inline const std::string & IParseData::meshPath(void) const
	{
		return _meshPath;
	}

	inline void IParseData::meshPath(std::string& path)
	{
		_meshPath = path;
	}

	inline void		IParseData::datasetType(IParseData::Type dataType) {
		_datasetType = dataType;
	}

	inline const std::vector<InputCamera::Ptr> IParseData::cameras(void) const
	{
		return _camInfos;
	}

	inline void IParseData::cameras(std::vector<InputCamera::Ptr>& cams)
	{
		_camInfos = cams;
	}

	inline const std::string IParseData::imgPath(void) const
	{
		return _imgPath;
	}

	inline void IParseData::imgPath(std::string& imPath)
	{
		_imgPath = imPath;
	}

	inline const IParseData::Type & IParseData::datasetType(void) const
	{
		return _datasetType;
	}

}