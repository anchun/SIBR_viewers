
#include "ParseData.hpp"
#include <iostream>
#include <string>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <map>

using namespace boost::algorithm;
namespace sibr {


	bool ParseData::parseBundlerFile(const std::string & bundler_file_path)
	{
		// check bundler file
		std::ifstream bundle_file(bundler_file_path);
		if (!bundle_file.is_open()) {
			return false;
		}

		// read number of images
		std::string line;
		getline(bundle_file, line);	// ignore first line - contains version

		bundle_file >> _numCameras;	// read first value (number of images)
		getline(bundle_file, line);	// ignore the rest of the line

		_outputCamsMatrix.resize(_numCameras);
		for (int i = 0; i < _numCameras; i++) {

			Matrix4f &m = _outputCamsMatrix[i];
			bundle_file >> m(0) >> m(1) >> m(2) >> m(3) >> m(4);
			bundle_file >> m(5) >> m(6) >> m(7) >> m(8) >> m(9);
			bundle_file >> m(10) >> m(11) >> m(12) >> m(13) >> m(14);
		}

		return true;
	}

	bool ParseData::parseColmapSparseData(const std::string & scene_sparse_path)
	{
		const std::string camerasListing = scene_sparse_path + "/cameras.txt";
		const std::string imagesListing = scene_sparse_path + "/images.txt";
		const std::string clippingPlanes = scene_sparse_path + "/../../../clipping_planes.txt";


		std::ifstream camerasFile(camerasListing);
		std::ifstream imagesFile(imagesListing);
		std::ifstream clippingPlanesFile(clippingPlanes);
		if (!camerasFile.is_open()) {
			SIBR_ERR << "Unable to load camera colmap file" << std::endl;
		}
		if (!imagesFile.is_open()) {
			SIBR_WRG << "Unable to load images colmap file" << std::endl;
		}

		std::string line;

		std::map<size_t, CameraParametersColmap> cameraParameters;

		while (std::getline(camerasFile, line)) {
			if (line.empty() || line[0] == '#') {
				continue;
			}

			std::vector<std::string> tokens = sibr::split(line, ' ');
			if (tokens.size() < 8) {
				SIBR_WRG << "Unknown line." << std::endl;
				continue;
			}
			if (tokens[1] != "PINHOLE" && tokens[1] != "OPENCV") {
				SIBR_WRG << "Unknown camera type." << std::endl;
				continue;
			}

			CameraParametersColmap params;
			params.id = std::stol(tokens[0]) - 1;
			params.width = std::stol(tokens[2]);
			params.height = std::stol(tokens[3]);
			params.fx = std::stof(tokens[4]);
			params.fy = std::stof(tokens[5]);
			params.dx = std::stof(tokens[6]);
			params.dy = std::stof(tokens[7]);

			cameraParameters[params.id] = params;

		}


		// Now load the individual images and their extrinsic parameters
		sibr::Matrix3f converter;
		converter << 1, 0, 0,
			0, -1, 0,
			0, 0, -1;

		int camid = 0;
		while (std::getline(imagesFile, line)) {
			if (line.empty() || line[0] == '#') {
				continue;
			}

			ImageListFile::Infos infos;

			std::vector<std::string> tokens = sibr::split(line, ' ');
			if (tokens.size() < 10) {
				SIBR_WRG << "Unknown line." << std::endl;
				continue;
			}

			size_t      id = std::stol(tokens[8]) - 1;
			float       qw = std::stof(tokens[1]);
			float       qx = std::stof(tokens[2]);
			float       qy = std::stof(tokens[3]);
			float       qz = std::stof(tokens[4]);
			float       tx = std::stof(tokens[5]);
			float       ty = std::stof(tokens[6]);
			float       tz = std::stof(tokens[7]);

			std::string imageName = tokens[9];

			if (cameraParameters.find(id) == cameraParameters.end())
			{
				SIBR_ERR << "Could not find intrinsics for image: "
					<< tokens[9] << std::endl;
			}
			const CameraParametersColmap & camParams = cameraParameters[id];

			/// camPos = translationM * rotationM * worldPos
			/// Since colmap data is exported in LHS and SIBR expects data in RHS, we need to apply a flip conversion to the coordinate system
			/// The flip matrix is given by converter [1 0 0; 0 -1 0; 0 0 -1]
			/// and the operation applied is:
			/// camPos = (flipM * translationM) * (flipM * rotationM) * worldPos
			/// Thus we store the camera matrix as: [orientation | translation]
			/// where,
			/// orientation = flipM * rotationM, and
			/// translation = flipM * translationM
			///
			/// For compatibility with FRIBR:
			/// orientation = flipM' * rotationM * flipM

			const sibr::Quaternionf quat(qw, qx, qy, qz);
			const sibr::Matrix3f orientation = /*converter.transpose() **/ quat.toRotationMatrix().transpose() * converter;
			sibr::Vector3f position(tx, ty, tz);

			// populate image infos
			infos.filename = imageName;
			infos.width = camParams.width;
			infos.height = camParams.height;

			_imgInfos.push_back(infos);

			// populate output camera matrices
			Matrix4f m;
			m(0) = camParams.fy;
			m(1) = 0.0f;
			m(2) = 0.0f;

			for (int i = 0; i < 9; i++) {
				m(3 + i) = orientation(i);
			}
			
			Vector3f finTrans = converter * position;
			for (int i = 0; i < 3; i++) {
				m(12 + i) = finTrans(i);
			}

			_outputCamsMatrix.push_back(m);

			// Skip the observations.
			camid++;
			std::getline(imagesFile, line);
		}

		_numCameras = camid;

		if (_activeImages.empty()) {
			_activeImages.resize(_imgInfos.size());

			for (int i = 0; i < _imgInfos.size(); i++)
				_activeImages[i] = true;
		}

		if (_excludeImages.empty()) {
			_excludeImages.resize(_imgInfos.size());

			for (int i = 0; i < _imgInfos.size(); i++)
				_excludeImages[i] = false;
		}

		if (_nearsFars.empty()) {
			_nearsFars.resize(_numCameras);

			if (!clippingPlanesFile.is_open()) {
				for (int i = 0; i < _numCameras; i++) {
					_nearsFars[i].near = 0.01f;
					_nearsFars[i].far = 1000.0f;
				}
			}
			else {
				int i = 0;
				while(std::getline(clippingPlanesFile, line)){
					std::vector<std::string> tokens = sibr::split(line, ' ');
					_nearsFars[i].near = stof(tokens[0]);
					_nearsFars[i].far = stof(tokens[1]);
					++i;
				}
			}

		}

		return true;
	}

	bool ParseData::parseSceneMetadata(const std::string & scene_metadata_path)
	{

		std::string line;
		std::vector<std::string> splitS;
		std::ifstream scene_metadata(scene_metadata_path);
		if (!scene_metadata.is_open()) {
			return false;
		}

		while (getline(scene_metadata, line))
		{
			//std::cout << line << '\n';
			if (line.compare("[list_images]") == 0) {
				getline(scene_metadata, line);	// ignore template specification line
				ImageListFile::Infos infos;
				int id;
				while (getline(scene_metadata, line))
				{
					//std::cout << line << std::endl;
					split(splitS, line, is_any_of(" "));
					//std::cout << splitS.size() << std::endl;
					if (splitS.size() > 1) {
						infos.filename = splitS[0];
						infos.width = stoi(splitS[1]);
						infos.height = stoi(splitS[2]);

						//infos.filename.erase(infos.filename.find_last_of("."), std::string::npos);
						id = atoi(infos.filename.c_str());

						_nearsFars.push_back(InputCamera::Z());
						if (splitS.size() > 3) {
							_nearsFars[id].near = stof(splitS[3]);
							_nearsFars[id].far = stof(splitS[4]);
						}
						else {
							_nearsFars[id].near = 0.1f;
							_nearsFars[id].far = 1.0f;
						}
						_imgInfos.push_back(infos);

						infos.filename.clear();
						splitS.clear();
					}
					else
						break;
				}
			}
			else if (line.compare("[active_images]") == 0) {

				getline(scene_metadata, line);	// ignore template specification line

				_activeImages.resize(_imgInfos.size());

				for (int i = 0; i < _imgInfos.size(); i++)
					_activeImages[i] = false;

				while (getline(scene_metadata, line))
				{
					int imageID;
					split(splitS, line, is_any_of(" "));
					//std::cout << splitS.size() << std::endl;
					if (splitS.size() > 1) {
						for (auto& s : splitS)
							_activeImages[stoi(s)] = true;
						splitS.clear();
					}
					else
						break;
				}
			}
			else if (line.compare("[exclude_images]") == 0) {

				getline(scene_metadata, line);	// ignore template specification line

				_excludeImages.resize(_imgInfos.size());

				for (int i = 0; i < _imgInfos.size(); i++)
					_excludeImages[i] = false;

				while (getline(scene_metadata, line))
				{
					int imageID;
					split(splitS, line, is_any_of(" "));
					//std::cout << splitS.size() << std::endl;
					if (splitS.size() > 1) {
						for (auto& s : splitS)
							_excludeImages[stoi(s)] = true;
						splitS.clear();
					}
					else
						break;
				}
			}
		}
		//std::cout << _imgInfos.size() << std::endl;
		if (_activeImages.empty()) {
			_activeImages.resize(_imgInfos.size());

			for (int i = 0; i < _imgInfos.size(); i++)
				_activeImages[i] = true;
		}

		if (_excludeImages.empty()) {
			_excludeImages.resize(_imgInfos.size());

			for (int i = 0; i < _imgInfos.size(); i++)
				_excludeImages[i] = false;
		}

		scene_metadata.close();

		return true;
	}


	void ParseData::getParsedBundlerData(const std::string & dataset_path, const std::string & scene_metadata_filename)
	{
		_basePathName = dataset_path;
		/*std::cout << scene_metadata_filename << std::endl;*/
		if (!parseSceneMetadata(dataset_path + "/" + scene_metadata_filename)) {
			SIBR_ERR << "Scene Metadata file does not exist at /" + dataset_path + "/." << std::endl;
		}

		if (!parseBundlerFile(dataset_path + "/cameras/bundle.out")) {
			SIBR_ERR << "Bundle file does not exist at /" + dataset_path + "/cameras/." << std::endl;
		}

		_meshPath = _basePathName + "/meshes/recon.ply";

		return;
	}

	void ParseData::getParsedColmapData(const std::string & dataset_path)
	{
		_basePathName = dataset_path + "/colmap/stereo";

		if (!parseColmapSparseData(_basePathName + "/sparse")) {
			SIBR_ERR << "Colmap Sparse Data text file does not exist at /" + dataset_path + "/sparse/." << std::endl;
		}

		_meshPath = _basePathName + "/../../capreal/mesh.ply";

	}

	void ParseData::getParsedData(const BasicIBRAppArgs & myArgs)
	{
		std::ifstream bundler(myArgs.dataset_path.get() + "/" + myArgs.scene_metadata_filename.get());
		if (bundler.good()) {
			getParsedBundlerData(myArgs.dataset_path, myArgs.scene_metadata_filename);
			_datasetType = Type::SIBR;
		}
		
		// What happens if both are present
		std::ifstream colmap(myArgs.dataset_path.get() + "/colmap/stereo/sparse/cameras.txt");

		if (colmap.good()) {
			getParsedColmapData(myArgs.dataset_path);
			_datasetType = Type::COLMAP;
		}

	}
}
