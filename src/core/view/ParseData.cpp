
#include "ParseData.hpp"

#include <fstream>
#include <sstream>


#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <map>
#include "core/system/String.hpp"


using namespace boost::algorithm;
namespace sibr {


	bool ParseData::parseBundlerFile(const std::string & bundler_file_path)
	{
		// check bundler file
		std::ifstream bundle_file(bundler_file_path);
		if (!bundle_file.is_open()) {
			SIBR_ERR << "Bundler file does not exist at " + bundler_file_path << std::endl;
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


		std::ifstream camerasFile(camerasListing);
		std::ifstream imagesFile(imagesListing);
		

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
			params.id = std::stol(tokens[0]);
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

			uint		cId = std::stoi(tokens[0]) - 1;
			float       qw = std::stof(tokens[1]);
			float       qx = std::stof(tokens[2]);
			float       qy = std::stof(tokens[3]);
			float       qz = std::stof(tokens[4]);
			float       tx = std::stof(tokens[5]);
			float       ty = std::stof(tokens[6]);
			float       tz = std::stof(tokens[7]);
			size_t      id = std::stol(tokens[8]);

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
			const sibr::Matrix3f orientation = quat.toRotationMatrix().transpose() * converter;
			sibr::Vector3f position(tx, ty, tz);

			// populate image infos
			infos.filename = imageName;
			infos.width = uint(camParams.width);
			infos.height = uint(camParams.height);
			infos.camId = cId;


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
			if (_excludeImages.empty()) {

				_activeImages.resize(_imgInfos.size());
				_excludeImages.resize(_imgInfos.size());

				for (int i = 0; i < _imgInfos.size(); i++) {
					_activeImages[i] = true;
					_excludeImages[i] = false;
				}
			}
			else {
				_activeImages.resize(_imgInfos.size());
				for (int i = 0; i < _imgInfos.size(); i++)
					_activeImages[i] = !_excludeImages[i];
			}
		}

		const std::string clippingPlanes = scene_sparse_path + "/../../../clipping_planes.txt";
		std::ifstream clippingPlanesFile(clippingPlanes);

		if (_nearsFars.empty()) {
			_nearsFars.resize(_numCameras);

			if (!clippingPlanesFile.is_open()) {
				for (int i = 0; i < _numCameras; i++) {
					_nearsFars[i].near = 0.1f;
					_nearsFars[i].far = 100.0f;
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

	bool ParseData::parseNVMData(const std::string & nvm_path)
	{

		const std::string clippingPlanes = nvm_path + "/clipping_planes.txt";

		std::ifstream nvm_scene(nvm_path + "/scene.nvm");
		std::ifstream clippingPlanesFile(clippingPlanes);

		std::string line;

		if (!nvm_scene.is_open()) {
			SIBR_ERR << "NVM scene.nvm file does not exist at /" + nvm_path << std::endl;
		}

		int rotation_parameter_num = 4;
		bool format_r9t = false;
		std::string token;
		if (nvm_scene.peek() == 'N')
		{
			nvm_scene >> token; //file header
			if (strstr(token.c_str(), "R9T"))
			{
				rotation_parameter_num = 9;    //rotation as 3x3 matrix
				format_r9t = true;
			}
		}

		nvm_scene >> _numCameras;	// read first value (number of images)

		_outputCamsMatrix.resize(_numCameras);
		_imgInfos.resize(_numCameras);

		sibr::ImageRGB image;
		
		std::function<Eigen::Matrix3f(const double[9])> matrix = [](const double q[9])
		{

			Eigen::Matrix3f m;
			double qq = sqrt(q[0] * q[0] + q[1] * q[1] + q[2] * q[2] + q[3] * q[3]);
			double qw, qx, qy, qz;
			if (qq > 0)
			{
				qw = q[0] / qq;
				qx = q[1] / qq;
				qy = q[2] / qq;
				qz = q[3] / qq;
			}
			else
			{
				qw = 1;
				qx = qy = qz = 0;
			}
			m(0, 0) = float(qw*qw + qx * qx - qz * qz - qy * qy);
			m(0, 1) = float(2 * qx*qy - 2 * qz*qw);
			m(0, 2) = float(2 * qy*qw + 2 * qz*qx);
			m(1, 0) = float(2 * qx*qy + 2 * qw*qz);
			m(1, 1) = float(qy*qy + qw * qw - qz * qz - qx * qx);
			m(1, 2) = float(2 * qz*qy - 2 * qx*qw);
			m(2, 0) = float(2 * qx*qz - 2 * qy*qw);
			m(2, 1) = float(2 * qy*qz + 2 * qw*qx);
			m(2, 2) = float(qz*qz + qw * qw - qy * qy - qx * qx);

			return m;
		};

		for (int i = 0; i < _numCameras; i++) {
			double f, q[9], c[3], d[2];
			Eigen::Matrix3f		matRotation;
			sibr::Matrix4f &m = _outputCamsMatrix[i];
			std::vector<std::string> splitS;

			nvm_scene >> token >> f;
			for (int j = 0; j < rotation_parameter_num; ++j) nvm_scene >> q[j];
			nvm_scene >> c[0] >> c[1] >> c[2] >> d[0] >> d[1];

			image.load(nvm_path + "/" + token);
			split(splitS, token, is_any_of("/"));

			_imgInfos[i].camId = i;
			_imgInfos[i].filename = splitS[1];
			_imgInfos[i].width = image.w();
			_imgInfos[i].height = image.h();

			m(0) = (float)f;

			m(1) = (float)d[0];
			m(2) = (float)d[1];

			Vector3f posCam((float)c[0], (float)c[1], (float)c[2]);

			Vector3f finCam;
			if (format_r9t)
			{

				std::cout << " WARNING THIS PART OF THE CODE WAS NEVER TESTED. IT IS SUPPOSED NOT TO WORK PROPERLY" << std::endl;
				matRotation <<
					float(q[0]), float(q[1]), float(q[2]),
					float(q[3]), float(q[4]), float(q[5]),
					float(q[6]), float(q[7]), float(q[8])
					;
			}
			else
			{

				Eigen::Matrix3f converter;
				converter <<
					1, 0, 0,
					0, -1, 0,
					0, 0, -1;
				//older format for compability
				Quaternionf quat((float)q[0], (float)q[1], (float)q[2], (float)q[3]);
				
				matRotation = quat.toRotationMatrix().transpose() * converter;
				finCam = -(matRotation * posCam);
			}



			for (int j = 0; j < 3; j++) m(12 + j) = (float)finCam[j];

			/*
			m(0) = focal length;
			m(1) = k_1;
			m(2) = k_2;
			m(3:11) = rotation matrix in RHS coordinate system;
			m(12:14) = trans; // trans = -rotation * camera center;
			*/

			for (int j = 0; j < 9; j++) m(3 + j) = (float)matRotation(j);

		}
		
		if (_activeImages.empty()) {
			if (_excludeImages.empty()) {

				_activeImages.resize(_imgInfos.size());
				_excludeImages.resize(_imgInfos.size());

				for (int i = 0; i < _imgInfos.size(); i++) {
					_activeImages[i] = true;
					_excludeImages[i] = false;
				}
			}
			else {
				_activeImages.resize(_imgInfos.size());
				for (int i = 0; i < _imgInfos.size(); i++)
					_activeImages[i] = !_excludeImages[i];
			}
		}

		if (_nearsFars.empty()) {
			_nearsFars.resize(_numCameras);

			if (!clippingPlanesFile.is_open()) {
				for (int i = 0; i < _numCameras; i++) {
					_nearsFars[i].near = 0.1f;
					_nearsFars[i].far = 100.0f;
				}
			}
			else {
				int i = 0;
				while (std::getline(clippingPlanesFile, line)) {
					std::vector<std::string> tokens = sibr::split(line, ' ');
					_nearsFars[i].near = stof(tokens[0]);
					_nearsFars[i].far = stof(tokens[1]);
					++i;
				}
			}

		}
		return true;
	}

	bool ParseData::parseMeshroomData(const std::string & sfm_path)
	{

		
		std::string file_path = sfm_path + "/" + sibr::listSubdirectories(sfm_path)[0] + "/cameras.sfm";
		std::string images_path = _basePathName + "/PrepareDenseScene/";

		std::ifstream json_file(file_path, std::ios::in);

		if (!json_file)
		{
			std::cerr << "file loading failed: " << file_path << std::endl;
			return false;
		}


		picojson::value v;
		picojson::set_last_error(std::string());
		std::string err = picojson::parse(v, json_file);
		if (!err.empty()) {
			picojson::set_last_error(err);
			json_file.setstate(std::ios::failbit);
		}

		picojson::array& views = v.get("views").get<picojson::array>();
		picojson::array& intrinsincs = v.get("intrinsics").get<picojson::array>();
		picojson::array& poses = v.get("poses").get<picojson::array>();

		_numCameras = poses.size();

		sibr::Matrix3f converter;
		converter << 1, 0, 0,
			0, -1, 0,
			0, 0, -1;

		size_t pose_idx, view_idx, intrinsic_idx;
		std::vector<std::string> splitS;

		for (size_t i = 0; i < _numCameras; ++i)
		{
			ImageListFile::Infos infos;
			Matrix4f m;
			//std::vector<std::string> splitS;


			pose_idx = i;
			std::string pose_id = poses[pose_idx].get("poseId").get<std::string>();
			
			for (size_t j = 0; j < views.size(); j++) {
				if (pose_id.compare(views[j].get("poseId").get<std::string>()) == 0) {
					view_idx = j;
					break;
				}
			}

			//picojson::value& view = views[i].get("value").get("ptr_wrapper").get("data");
			std::string intrinsics_id = views[view_idx].get("intrinsicId").get<std::string>();

			for (size_t k = 0; k < intrinsincs.size(); k++) {
				if (intrinsics_id.compare(intrinsincs[k].get("intrinsicId").get<std::string>()) == 0) {
					intrinsic_idx = k;
					break;
				}
			}

			//picojson::value& intrinsic = intrinsincs[intrinsic_idx].get("value").get("ptr_wrapper").get("data");
			m(0) = std::stof(intrinsincs[intrinsic_idx].get("pxFocalLength").get<std::string>());
			//float dx = std::stof(intrinsincs[intrinsic_idx].get("principalPoint").get<picojson::array>()[0].get<std::string>());
			//float dy = std::stof(intrinsincs[intrinsic_idx].get("principalPoint").get<picojson::array>()[1].get<std::string>());

			//Eigen::Vector3f k(0.0f, 0.0f, 0.0f);

			//if (!intrinsincs[intrinsic_idx].get("distortionParams").is<picojson::null>())
			//	k = Eigen::Vector3f(intrinsincs[intrinsic_idx].get("distortionParams").get<picojson::array>()[0].get<double>(),
			//		intrinsincs[intrinsic_idx].get("distortionParams").get<picojson::array>()[1].get<double>(),
			//		intrinsincs[intrinsic_idx].get("distortionParams").get<picojson::array>()[2].get<double>());

			//m(1) = std::stof(intrinsincs[intrinsic_idx].get("distortionParams").get<picojson::array>()[0].get<std::string>());
			//m(2) = std::stof(intrinsincs[intrinsic_idx].get("distortionParams").get<picojson::array>()[1].get<std::string>());
			m(1) = 0;
			m(2) = 0;

			infos.filename = "/../PrepareDenseScene/" + sibr::listSubdirectories(images_path)[0] + "/" + pose_id + ".exr";
			infos.width = std::stoi(views[view_idx].get("width").get<std::string>());
			infos.height = std::stoi(views[view_idx].get("height").get<std::string>());
			
			
			//std::string path = views[view_idx].get("path").get<std::string>();
			//split(splitS, path, is_any_of("/"));
			//std::string filename = splitS[splitS.size() - 1];
			//infos.filename = "/../Images/" + filename;
			//split(splitS, filename, is_any_of("."));

			//infos.camId = stoi(splitS[0]);
			infos.camId = i;

			picojson::array& center = poses[pose_idx].get("pose").get("transform").get("center").get<picojson::array>();
			picojson::array& rotation = poses[pose_idx].get("pose").get("transform").get("rotation").get<picojson::array>();

			std::vector<Eigen::Vector3f> rows;
			Eigen::Vector3f row;
			Eigen::Vector3f position(std::stof(center[0].get<std::string>()), std::stof(center[1].get<std::string>()), std::stof(center[2].get<std::string>()));
			Eigen::Matrix3f orientation;

			for (int ii = 0; ii < 3; ++ii) {
				for (int jj = 0; jj < 3; ++jj)
					row(jj) = std::stof(rotation[jj + ii * 3].get<std::string>());
				rows.push_back(row);
			}

			orientation.row(0) = rows[0];
			orientation.row(1) = -rows[1];
			orientation.row(2) = -rows[2];

			for (int i = 0; i < 9; i++) {
				m(3 + i) = orientation(i);
			}

			Vector3f finTrans = -orientation * position;
			for (int i = 0; i < 3; i++) {
				m(12 + i) = finTrans(i);
			}

			/*
			m(0) = focal length;
			m(1) = k_1;
			m(2) = k_2;
			m(3:11) = rotation matrix in RHS coordinate system;
			m(12:14) = trans; // trans = -rotation * camera center;
			*/

			_outputCamsMatrix.push_back(m);
			_imgInfos.push_back(infos);
		}

		if (_activeImages.empty()) {
			if (_excludeImages.empty()) {

				_activeImages.resize(_imgInfos.size());
				_excludeImages.resize(_imgInfos.size());

				for (int i = 0; i < _imgInfos.size(); i++) {
					_activeImages[i] = true;
					_excludeImages[i] = false;
				}
			}
			else {
				_activeImages.resize(_imgInfos.size());
				for (int i = 0; i < _imgInfos.size(); i++)
					_activeImages[i] = !_excludeImages[i];
			}
		}

		const std::string clippingPlanes = _basePathName + "/../clipping_planes.txt";
		std::ifstream clippingPlanesFile(clippingPlanes);
		std::string line;

		if (_nearsFars.empty()) {
			_nearsFars.resize(_numCameras);

			if (!clippingPlanesFile.is_open()) {
				for (int i = 0; i < _numCameras; i++) {
					_nearsFars[i].near = 0.1f;
					_nearsFars[i].far = 100.0f;
				}
			}
			else {
				int i = 0;
				while (std::getline(clippingPlanesFile, line)) {
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

		uint camId = 0;
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
						infos.camId = camId;

						//infos.filename.erase(infos.filename.find_last_of("."), std::string::npos);
						id = atoi(infos.filename.c_str());

						InputCamera::Z nearFar;
						
						if (splitS.size() > 3) {
							nearFar.near = stof(splitS[3]);
							nearFar.far = stof(splitS[4]);
						}
						else {
							nearFar.near = 0.1f;
							nearFar.far = 1.0f;
						}
						_imgInfos.push_back(infos);
						_nearsFars.push_back(nearFar);

						++camId;
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
					split(splitS, line, is_any_of(" "));
					//std::cout << splitS.size() << std::endl;
					if (splitS.size() > 1) {
						for (auto& s : splitS)
							if (!s.empty())
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
					split(splitS, line, is_any_of(" "));
					//std::cout << splitS.size() << std::endl;
					if (splitS.size() > 1) {
						for (auto& s : splitS)
							if(!s.empty())
								_excludeImages[stoi(s)] = true;
						splitS.clear();
					}
					else
						break;
				}
			}
			else if (line == "[proxy]") {
				// Read the relative path of the mesh to load.
				getline(scene_metadata, line);
				_meshPath = _basePathName + "/" + line;
			}
		}

		if (_activeImages.empty()) {
			if (_excludeImages.empty()) {

				_activeImages.resize(_imgInfos.size());
				_excludeImages.resize(_imgInfos.size());

				for (int i = 0; i < _imgInfos.size(); i++) {
					_activeImages[i] = true;
					_excludeImages[i] = false;
				}
			}
			else {
				_activeImages.resize(_imgInfos.size());
				for (int i = 0; i < _imgInfos.size(); i++)
					_activeImages[i] = !_excludeImages[i];
			}
		}

		if (_activeImages.size() != _excludeImages.size())
			SIBR_ERR << "Active Image size does not match exclude image size" << std::endl;

		scene_metadata.close();

		return true;
	}

	void ParseData::getParsedBundlerData(const std::string & dataset_path, const std::string & customPath, const std::string & scene_metadata_filename)
	{
		_basePathName = dataset_path + customPath;
		/*std::cout << scene_metadata_filename << std::endl;*/
		if (!parseSceneMetadata(_basePathName + "/" + scene_metadata_filename)) {
			SIBR_ERR << "Scene Metadata file does not exist at /" + _basePathName + "/." << std::endl;
		}

		if (!parseBundlerFile(_basePathName + "/cameras/bundle.out")) {
			SIBR_ERR << "Bundle file does not exist at /" + _basePathName + "/cameras/." << std::endl;
		}
		// Default mesh path if none found in the metadata file.
		if (_meshPath.empty()) {
			_meshPath = _basePathName + "/meshes/recon";
		}

	}

	void ParseData::getParsedMeshroomData(const std::string & dataset_path, const std::string & customPath)
	{
		
		_basePathName = dataset_path;// +"/MeshroomCache/";

		if (!parseMeshroomData(_basePathName + "/StructureFromMotion/")) {
			SIBR_ERR << "Meshroom cameras.sfm file does not exist at /" + _basePathName + "/StructureFromMotion/<cache_dir>/." << std::endl;
		}

		_meshPath = _basePathName + "/Texturing/" + sibr::listSubdirectories(_basePathName + "/Texturing/")[0] + "/texturedMesh.obj";

	}

	void ParseData::getParsedColmapData(const std::string & dataset_path)
	{
		_basePathName = dataset_path + "/colmap/stereo";

		if (!parseColmapSparseData(_basePathName + "/sparse")) {
			SIBR_ERR << "Colmap Sparse Data text file does not exist at /" + _basePathName + "/sparse/." << std::endl;
		}

		_meshPath = _basePathName + "/../../capreal/mesh.ply";

	}

	void ParseData::getParsedNVMData(const std::string & dataset_path, const std::string & customPath, const std::string & nvm_path)
	{
		_basePathName = dataset_path + customPath + nvm_path;

		if (!parseNVMData(_basePathName)) {
			SIBR_ERR << "NVM folder does not exist at /" + _basePathName << std::endl;
		}

		_meshPath = dataset_path + "/capreal/mesh.ply";
	}

	void ParseData::getParsedData(const BasicIBRAppArgs & myArgs, const std::string & customPath)
	{
		std::string bundler = myArgs.dataset_path.get() + customPath + "/cameras/bundle.out";
		std::string nvmscene = myArgs.dataset_path.get() + customPath + "/nvm/";
		std::string colmap = myArgs.dataset_path.get() + "/colmap/stereo/sparse/";
		std::string meshroom = myArgs.dataset_path.get() + "/StructureFromMotion/";


		if (sibr::fileExists(bundler)) {
			getParsedBundlerData(myArgs.dataset_path, customPath, myArgs.scene_metadata_filename);
			_datasetType = Type::SIBR;
		}else if (sibr::directoryExists(colmap)) {
			getParsedColmapData(myArgs.dataset_path);
			_datasetType = Type::COLMAP;
		}
		else if (sibr::directoryExists(nvmscene)) {
			getParsedNVMData(myArgs.dataset_path, customPath, "/nvm/");
			_datasetType = Type::NVM;
		}
		else if (sibr::directoryExists(meshroom)) {
			getParsedMeshroomData(myArgs.dataset_path);
			_datasetType = Type::MESHROOM;
		}
		else {
			SIBR_ERR << "Cannot determine type of dataset at /" + myArgs.dataset_path.get() + customPath << std::endl;
		}
		
		// What happens if multiple are present?
		// Ans: Priority --> SIBR > COLMAP > NVM
		
		// Find max cam ID and check present image IDs
		int maxId = 0;
		std::vector<bool> presentIDs;
		
		presentIDs.resize(_numCameras);

		for (int c = 0; c < _numCameras; c++) {
			maxId = (maxId > int(_imgInfos[c].camId)) ? maxId : int(_imgInfos[c].camId);
			try
			{
				presentIDs[_imgInfos[c].camId] = true;
			}
			catch (const std::exception&)
			{
				SIBR_ERR << "Incorrect Camera IDs " << std::endl;
			}
		}

		// Check if max cam ID matches max number of cams
		// If not find the missing IDs 
		std::vector<int> missingIDs;
		int curid;
		int j, pos;
		if (maxId >= _numCameras) {
			for (int i = 0; i < _numCameras; i++) {
				if (!presentIDs[i]) { missingIDs.push_back(i); }
			}

			// Now, shift the imgInfo IDs to adjust max Cam IDs
			for (int k = 0; k < _numCameras; k++) {
				curid = _imgInfos[k].camId;
				pos = -1;
				for (j = 0; j < missingIDs.size(); j++) {
					if (curid > missingIDs[j]) { pos = j; }
					else { break; }
				}

				_imgInfos[k].camId = _imgInfos[k].camId - (pos + 1);
			}
		}

	}
}
