
#ifndef __SIBR_VIEW_UISHORTCUTS_HPP__
# define __SIBR_VIEW_UISHORTCUTS_HPP__

# include <unordered_map>
# include "core/view/Config.hpp"

namespace sibr
{
	/**
	* \ingroup sibr_view
	*/
	class SIBR_VIEW_EXPORT UIShortcuts
	{
	public:

		static UIShortcuts& global( void );

		void	list( void );
		void	add( const std::string& shortcut, const char* desc );


	private:
		std::unordered_map<std::string, const char*>	_shortcuts;

	};

	///// DEFINITIONS /////

} // namespace sibr

#endif // __SIBR_VIEW_UISHORTCUTS_HPP__
