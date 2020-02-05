
#ifndef __SIBR_VIEW_UISHORTCUTS_HPP__
# define __SIBR_VIEW_UISHORTCUTS_HPP__

# include <unordered_map>
# include "core/view/Config.hpp"

namespace sibr
{
	/** Register and display keyboard shortcuts.
	* \todo The system should be more robust for collision detection.
	* \ingroup sibr_view
	*/
	class SIBR_VIEW_EXPORT UIShortcuts
	{
	public:

		/** Singleton. */
		static UIShortcuts& global( void );

		/** Print all registered shortcuts. */
		void	list( void );

		/** Register a shortcut.
		 *\param shortcut the shortcut keys
		 *\param desc the description
		 */
		void	add( const std::string& shortcut, const char* desc );


	private:
		std::unordered_map<std::string, const char*>	_shortcuts; ///< List of shortcuts.

	};


} // namespace sibr

#endif // __SIBR_VIEW_UISHORTCUTS_HPP__
