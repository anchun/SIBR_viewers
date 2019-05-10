#pragma once

#include "core/view/Config.hpp"

#include <vector>
#include <string>

namespace sibr
{

	/** Singleton used to store a list of plausible path to look for (based on the ibr_resources.ini)
	\ingroup sibr_view
	*/
	class SIBR_VIEW_EXPORT Resources
	{
	public:
		/// Our singleton
		static Resources* Instance();

	protected:
		Resources();
		virtual ~Resources();

	public:
		/** Look for the filename into plausible _rscPaths to return rebuilt pathName*/
		std::string getResourceFilePathName(std::string const & filename, bool & success);
		std::string getResourceFilePathName(std::string const & filename);

	protected:
		/** stock the resources path to find any file */
		std::vector<std::string>    _rscPaths;
		static Resources *          _instance;
	};

} // namespace sibr
