
#ifndef __SIBR_ASSETS_CONFIG_HPP__
# define __SIBR_ASSETS_CONFIG_HPP__

# include "core/graphics/Config.hpp"
# include <iomanip>


#ifdef SIBR_OS_WINDOWS
//// Export Macro (used for creating DLLs) ////
# ifdef SIBR_STATIC_DEFINE
#   define SIBR_EXPORT
#   define SIBR_NO_EXPORT
# else
#   ifndef SIBR_ASSETS_EXPORT
#     ifdef SIBR_ASSETS_EXPORTS
         /* We are building this library */
#       define SIBR_ASSETS_EXPORT __declspec(dllexport)
#     else
         /* We are using this library */
#       define SIBR_ASSETS_EXPORT __declspec(dllimport)
#     endif
#   endif
#   ifndef SIBR_NO_EXPORT
#     define SIBR_NO_EXPORT
#   endif
# endif
# else
#  define SIBR_ASSETS_EXPORT
# endif

namespace sibr
{
	/**
	* Utility that converts an integer id to a string using
	* the "most used" format.
	*       \param id the id to convert (fi 7)
	*       \return the corresponding string (fi "0000007")
	*/
	inline std::string		imageIdToString( int id ) {
		std::ostringstream oss;
		oss << std::setfill('0') << std::setw(8) << id;
		return oss.str();
	}

	/** Generate a string representation of an integer, padded with zeros.
	 * \param id the integer
	 * \return the padded string
	 * \note The template int value determines the padding count.
	 * */
	template<unsigned int N> std::string intToString(int id) {
		std::ostringstream oss;
		oss << std::setfill('0') << std::setw(N) << id;
		return oss.str();
	}

	/**
	* Get the default path and filename used for the proxy
	* mesh.
	* \param datasetPath the base path
	* \return the mesh path
	*/
	inline std::string		getProxyFilename( const std::string& datasetPath ) {
		return datasetPath + "/pmvs/models/pmvs_recon.ply";
	}

	/**
	 * Loading status for streaming.
	* \todo Rename the following status into: NotLoaded, CPULoading, CPUReady, GPUReady, Failure.
	*/
	namespace LoadingStatus
	{
		enum	Enum
		{
			NotLoaded = 0,
			InProgress,
			CPUReady,
			Successful,
			Failure,

			Count
		};
	} // namespace LoadingStatus

} // namespace sibr

#endif // __SIBR_ASSETS_CONFIG_HPP__
