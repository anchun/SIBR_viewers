
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>
#include "core/view/Resources.hpp"

/// \todo TODO: If you care about security (did someone want to hack/use your app
/// to hide a virus/retrieve informations from this compiled code), comment
/// the following line and resolve warnings by finding new safe-functions.
#pragma warning(disable:4996) // affect this .cpp only

namespace sibr
{

	Resources* Resources::_instance = NULL;

	Resources* Resources::Instance()
	{
		if (_instance == 0)
			_instance = new Resources;
		return _instance;
	}

	Resources::Resources()
	{
		_rscPaths.push_back(".");
		// Support for new shaders installation script w/o having to specify sub-directory. (SR)
		_rscPaths.push_back(sibr::getBinDirectory() + "/shaders_rsc/");
		std::ifstream rscFile("ibr_resources.ini");
		if(rscFile.good())
		{
			for(std::string line; std::getline(rscFile, line); )
			{
				if (line.at(line.length()-1) != '/')
					line.append("/");
				_rscPaths.push_back(line);
			}
		}

		/// \todo WIP: used in previsoin to load plugins (TODO: test under linux)
		std::ifstream pathFile("ibr_paths.ini");
		if(pathFile.good())
		{
			for(std::string line; std::getline(pathFile, line); )
			{
				std::string name    = line.substr(0, line.find("="));
				std::string value   = line.substr(line.find("=")+1, line.length());
				char* curEnv = getenv(name.c_str());
				std::string currentEnv;
				if(curEnv!=NULL)
					currentEnv = std::string(curEnv);
#ifdef _WIN32
				std::replace(value.begin(), value.end(), '/', '\\'); // linux to windows path
				char delimiter = ';';
#else
				std::replace(value.begin(), value.end(), '\\', '/'); // windows to linux path
				char delimiter = ':';
#endif
				std::stringstream ss;
				ss << delimiter;
				if(!currentEnv.empty())
					if (currentEnv.at(currentEnv.length()-1) != delimiter)
						currentEnv.append(ss.str());    

				line = name + "=" + currentEnv + value;
				putenv(const_cast<char*>(line.c_str()));

				std::cout<<"[Resources] env: "<<name<<"="<<getenv(name.c_str())<<std::endl;
			}
		}
	}

	Resources::~Resources()
	{
	}

	std::string Resources::getResourceFilePathName(std::string const & filename, bool & success)
	{
		// we assume the first element of _rscPaths if the current dir
		for(std::string rscPath : _rscPaths)
		{
			std::string filePathName  = rscPath + "/" + filename;
			//SIBR_LOG << "Checking file path: " << filePathName << "\t for filename: " << filename << std::endl;
			std::ifstream rscFile(filePathName);
			if (success = rscFile.good()) {
				//SIBR_LOG << "Shader file found at path: " << filePathName << "\tfilename: " << filename << std::endl;
				//success = true;
				return filePathName;
			}
		}
		return filename;
	}

	std::string Resources::getResourceFilePathName(std::string const & filename)
	{
		bool success = false;
		return getResourceFilePathName(filename,success);
	}

} // namespace sibr
