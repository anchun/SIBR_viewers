
#ifndef __SIBR_SYSTEM_STRING_HPP__
# define __SIBR_SYSTEM_STRING_HPP__

# include "core/system/Config.hpp"

// See sibr/system/Utils.hpp for other String functions
// (String functions should be moved here)

namespace sibr
{
	/**
	* \ingroup sibr_system
	*/
	SIBR_SYSTEM_EXPORT std::string strSearchAndReplace( const std::string& src, const std::string& search, const std::string& replaceby );

	/**
	* \ingroup sibr_system
	*/
	SIBR_SYSTEM_EXPORT bool strContainsOnlyDigits(const std::string& str);


} // namespace sibr

#endif // __SIBR_SYSTEM_STRING_HPP__
