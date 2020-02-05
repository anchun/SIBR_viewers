
#ifndef __SIBR_ASSETS_IFILELOADER_HPP__
# define __SIBR_ASSETS_IFILELOADER_HPP__

# include "core/assets/Config.hpp"

namespace sibr
{
	/**
	General file loading interface.
	\ingroup sibr_assets
	*/
	class SIBR_ASSETS_EXPORT IFileLoader
	{
	public:

		/** Destructor. */
		virtual ~IFileLoader( void ) { }

		/** Load the file content from disk.
		\param filename path to the file
		\param verbose display information
		\return a boolean denoting success
		*/
		virtual bool load( const std::string& filename, bool verbose = true ) = 0;
	};

} // namespace sibr

#endif // __SIBR_ASSETS_IFILELOADER_HPP__
