
#include "ParseData.hpp"
#include <iostream>
#include <string>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

using namespace boost::algorithm;
namespace sibr {
	void ParseData::getParsedData(const std::string & dataset_path, const std::string & scene_metadata_filename)
	{
		_basePathName = dataset_path;
		/*std::cout << scene_metadata_filename << std::endl;*/
		if (!parseSceneMetadata(dataset_path + "/" + scene_metadata_filename)) {
			SIBR_ERR << "Scene Metadata file does not exist at \"" + dataset_path + "\"." << std::endl;
		}

		if (!parseBundlerFile(dataset_path + "/cameras/bundle.out")) {
			SIBR_ERR << "Bundle file does not exist at \"" + dataset_path + "\cameras\"." << std::endl;
		}

		return;
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
						for (auto& s: splitS)
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
		if (_activeImages.size() == 0) {
			_activeImages.resize(_imgInfos.size());

			for (int i = 0; i < _imgInfos.size(); i++)
				_activeImages[i] = true;
		}

		if (_excludeImages.size() == 0) {
			_excludeImages.resize(_imgInfos.size());

			for (int i = 0; i < _imgInfos.size(); i++)
				_excludeImages[i] = false;
		}

		scene_metadata.close();

		return true;
	}

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
}
