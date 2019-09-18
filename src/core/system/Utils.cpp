
#include <boost/filesystem.hpp>
#include <fstream>
#include <sstream>
#include <vector>
#include <Windows.h>
#include <nfd.h>
#include "core/system/Utils.hpp"

namespace sibr
{
	std::string	loadFile(const std::string& fname)
	{
		std::ifstream file(fname.c_str(), std::ios::binary);
		if (!file || !file.is_open()) {
			SIBR_ERR << "File not found: " << fname << std::endl;
			return "";
		}
		file.seekg(0, std::ios::end);

		std::streampos length = file.tellg();
		file.seekg(0, std::ios::beg);

		std::vector<char> buffer(length);
		file.read(&buffer[0], length);
		file.close();

		return std::string(buffer.begin(), buffer.end());
	}

	void			makeDirectory(const std::string& path)
	{
		boost::filesystem::path p(path);
		if (boost::filesystem::exists(p) == false)
			boost::filesystem::create_directories(p);
	}

	std::vector<std::string> listFiles(const std::string & path, const bool listHidden, const bool includeSubdirectories, const std::vector<std::string> & allowedExtensions)
	{
		if (!directoryExists(path)) {
			return {};
		}

		std::vector<std::string> files;
		bool shouldCheckExtension = !allowedExtensions.empty();

		try {
			boost::filesystem::directory_iterator end_iter;
			for (boost::filesystem::directory_iterator dir_itr(path); dir_itr != end_iter; ++dir_itr) {

				const std::string itemName = dir_itr->path().filename().string();
				if (includeSubdirectories && boost::filesystem::is_directory(dir_itr->status())) {
					if (listHidden || (itemName.size() > 0 && itemName.at(0) != '.')) {
						files.push_back(itemName);
					}
				}
				else if (boost::filesystem::is_regular_file(dir_itr->status())) {
					bool shouldKeep = !shouldCheckExtension;
					if (shouldCheckExtension) {
						for (const auto & allowedExtension : allowedExtensions) {
							if (dir_itr->path().extension() == ("." + allowedExtension) || dir_itr->path().extension() == allowedExtension) {
								shouldKeep = true;
								break;
							}
						}
					}

					if (shouldKeep && (listHidden || (itemName.size() > 0 && itemName.at(0) != '.'))) {
						files.push_back(itemName);
					}
				}
			}
		}
		catch (const boost::filesystem::filesystem_error&) {
			std::cout << "Can't access or find directory." << std::endl;
		}

		std::sort(files.begin(), files.end());

		return files;
	}

	std::vector<std::string> listSubdirectories(const std::string & path, const bool listHidden)
	{
		if (!directoryExists(path)) {
			return {};
		}

		std::vector<std::string> dirs;


		try {
			boost::filesystem::directory_iterator end_iter;
			for (boost::filesystem::directory_iterator dir_itr(path); dir_itr != end_iter; ++dir_itr) {

				const std::string itemName = dir_itr->path().filename().string();
				if (boost::filesystem::is_directory(dir_itr->status())) {
					if (listHidden || (itemName.size() > 0 && itemName.at(0) != '.')) {
						dirs.push_back(itemName);
					}
				}
			}
		}
		catch (const boost::filesystem::filesystem_error& ) {
			std::cout << "Can't access or find directory." << std::endl;
		}

		std::sort(dirs.begin(), dirs.end());

		return dirs;
	}


	bool copyDirectory(const std::string& src, const std::string& dst)
	{
		boost::filesystem::path source = src;
		boost::filesystem::path destination = dst;
		namespace fs = boost::filesystem;
		try
		{
			// Check whether the function call is valid
			if (!fs::exists(source) || !fs::is_directory(source))
			{
				std::cerr << "Source directory " << source.string()
					<< " does not exist or is not a directory." << '\n'
					;
				return false;
			}
			if (fs::exists(destination))
			{
				std::cerr << "Destination directory " << destination.string()
					<< " already exists." << '\n'
					;
				return false;
			}
			// Create the destination directory
			if (!fs::create_directory(destination))
			{
				std::cerr << "Unable to create destination directory"
					<< destination.string() << '\n'
					;
				return false;
			}
		}
		catch (fs::filesystem_error const & e)
		{
			std::cerr << e.what() << '\n';
			return false;
		}
		// Iterate through the source directory
		for (fs::directory_iterator file(source); file != fs::directory_iterator(); ++file)
		{
			try
			{
				fs::path current(file->path());
				if (fs::is_directory(current))
				{
					// Found directory: Recursion
					if (!copyDirectory(current.string(), (destination / current.filename()).string()))
					{
						return false;
					}
				}
				else
				{
					// Found file: Copy
					fs::copy_file(
						current,
						destination / current.filename()
					);
				}
			}
			catch (fs::filesystem_error const & e)
			{
				std::cerr << e.what() << '\n';
			}
		}
		return true;
	}

	bool copyFile(const std::string & src, const std::string & dst, const bool overwrite)
	{
		boost::filesystem::path source = src;
		boost::filesystem::path destination = dst;
		namespace fs = boost::filesystem;
		try {
			// Check whether the function call is valid
			if (!fs::exists(source) || !fs::is_regular_file(source))
			{
				std::cerr << "Source file " << source.string()
					<< " does not exist or is not a regular file." << '\n'
					;
				return false;
			}

			// If the destination is a directory, we copy the file into this directory, with the same name.
			if (fs::is_directory(destination)) {
				destination = destination / source.filename();
			}

			if (fs::exists(destination) && !overwrite)
			{
				std::cerr << "Destination file " << destination.string()
					<< " already exists." << '\n'
					;
				return false;
			}
			if(overwrite) {
				fs::copy_file(source, destination, boost::filesystem::copy_option::overwrite_if_exists);
			} else {
				fs::copy_file(source, destination);
			}

		}
		catch (fs::filesystem_error const & e)
		{
			std::cerr << e.what() << '\n';
			return false;
		}

		return true;
	}

	void			emptyDirectory(const std::string& path) {
		boost::filesystem::path p(path);
		for (boost::filesystem::directory_iterator end_dir_it, it(p); it != end_dir_it; ++it) {
			boost::filesystem::remove_all(it->path());
		}
	}

	bool			fileExists(const std::string& path)
	{
		boost::filesystem::path p(path);
		return boost::filesystem::exists(p) && boost::filesystem::is_regular_file(path);
	}

	bool			directoryExists(const std::string& path)
	{
		boost::filesystem::path p(path);
		return boost::filesystem::exists(p) && boost::filesystem::is_directory(path);
	}

	size_t getAvailableMem() {
#define DIV 1024
		MEMORYSTATUSEX statex;
		statex.dwLength = sizeof(statex);
		GlobalMemoryStatusEx(&statex);
		return static_cast<size_t>(statex.ullAvailPhys) / DIV;
	}

	SIBR_SYSTEM_EXPORT std::string getBinDirectory()
	{
		char exePath[4095];

		unsigned int len = GetModuleFileNameA(GetModuleHandleA(0x0), exePath, MAX_PATH);
		if (len == 0) // memory not sufficient or general error occured
		{
			SIBR_ERR << "Can't find binary folder! Please specify as command-line option using --appPath option!" << std::endl;
		}
		return parentDirectory(exePath);
	}

	bool showFilePicker(std::string & selectedElement,
		const FilePickerMode mode, const std::string & directoryPath, const std::string & extensionsAllowed) {
		nfdchar_t *outPath = NULL;
		nfdresult_t result = NFD_CANCEL;
		
		if (mode == Directory) {
			result = NFD_PickFolder(directoryPath.c_str(), &outPath);
		} else if (mode == Save) {
			result = NFD_SaveDialog(extensionsAllowed.empty() ? NULL : extensionsAllowed.c_str(), directoryPath.c_str(), &outPath);
		} else {
			result = NFD_OpenDialog(extensionsAllowed.empty() ? NULL : extensionsAllowed.c_str(), directoryPath.c_str(), &outPath);
		}


		if (result == NFD_OKAY) {
			selectedElement = std::string(outPath);
			free(outPath);
			return true;
		} else if (result == NFD_CANCEL) {
			// User canceled, do nothing.
		} else {
			// Programmatic error.
			SIBR_WRG << "Unable to present file dialog." << std::endl;
			std::cout << std::string(NFD_GetError()) << std::endl;
		}
		free(outPath);
		return false;

	}

} // namespace sirb
