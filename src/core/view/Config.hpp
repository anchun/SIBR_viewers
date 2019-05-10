
#ifndef __SIBR_VIEW_CONFIG_HPP__
# define __SIBR_VIEW_CONFIG_HPP__

# include "core/graphics/Config.hpp"


//// Export Macro (used for creating DLLs) ////
# ifdef SIBR_OS_WINDOWS
#  ifdef SIBR_STATIC_VIEW_DEFINE
#    define SIBR_VIEW_EXPORT
#    define SIBR_NO_VIEW_EXPORT
#  else
#    ifndef SIBR_VIEW_EXPORT
#      ifdef SIBR_VIEW_EXPORTS
          /* We are building this library */
#        define SIBR_VIEW_EXPORT __declspec(dllexport)
#      else
          /* We are using this library */
#        define SIBR_VIEW_EXPORT __declspec(dllimport)
#      endif
#    endif
#    ifndef SIBR_NO_EXPORT
#      define SIBR_NO_EXPORT 
#    endif
#  endif
# else
#  define SIBR_VIEW_EXPORT
# endif


#endif // __SIBR_VIEW_CONFIG_HPP__
