
#ifndef __SIBR_SYSTEM_UTILS_HPP__
# define __SIBR_SYSTEM_UTILS_HPP__

# include <vector>
# include "core/system/Config.hpp"

namespace sibr
{
	/**
	* \addtogroup sibr_system
	* @{
	*/

	/** load the whole file into a std::string */
	SIBR_SYSTEM_EXPORT std::string	loadFile( const std::string& filename );

	/** split string into sub-strings delimited by \param delim*/
	SIBR_SYSTEM_EXPORT std::vector<std::string>	split( const std::string& str, char delim='\n' );

	/** Create directory (if it doesn't exist already) */
	SIBR_SYSTEM_EXPORT void			makeDirectory( const std::string& path );

	/**List content of directory. */
	SIBR_SYSTEM_EXPORT std::vector<std::string>	listFiles(const std::string & path, const bool listHidden = false, const bool includeSubdirectories = false, const std::vector<std::string> & allowedExtensions = {});

	/**List content of directory, including subdirectories. */
	SIBR_SYSTEM_EXPORT std::vector<std::string>	listSubdirectories(const std::string& path, const bool listHidden = false);

	/**Copy directory. */
	SIBR_SYSTEM_EXPORT bool copyDirectory(const std::string& src, const std::string& dst);

	/**Copy file. */
	SIBR_SYSTEM_EXPORT bool copyFile(const std::string& src, const std::string& dst, const bool overwrite = false);

	/** Empty a directory (if it exist already) */
	SIBR_SYSTEM_EXPORT void			emptyDirectory(const std::string& path);

	/** Return true if file exists*/
	SIBR_SYSTEM_EXPORT bool			fileExists( const std::string& path );

	/** Return true if directory exists*/
	SIBR_SYSTEM_EXPORT bool			directoryExists( const std::string& path );

	/** Return the available memory on windows system in Ko*/
	SIBR_SYSTEM_EXPORT size_t			getAvailableMem();

    /** Wrapper around sibr::sprintf that returns a string */
	SIBR_SYSTEM_EXPORT std::string	sprint( const char *msg, ... );
	SIBR_SYSTEM_EXPORT int 		sprintf( char* buffer, size_t size, const char* format, ... );

	/** Selection mode for the file picker. */
	enum FilePickerMode {
		Default, Save, Directory
	};

	/**
	 * Present a native OS file picker.
	 * \param selectedElement will contain the path to the element selected by the user if any.
	 * \param mode the mode to use, pick from Save, Directory, Default.
	 * \param directoryPath the initial directory to present to the user. \warn '.' relative path is unsupported.
	 * \param extensionsAllowed a list of file extensions to allow: "obj,ply" for instance.
	 * \return true if an element was selected, else false.
	 */
	SIBR_SYSTEM_EXPORT bool showFilePicker(std::string & selectedElement,
		const FilePickerMode mode, const std::string & directoryPath = "", const std::string & extensionsAllowed = "");

	/*** @} */
} // namespace sibr

#endif // __SIBR_SYSTEM_UTILS_HPP__
