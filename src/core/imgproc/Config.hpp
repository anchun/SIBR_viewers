
#ifndef __SIBR_IMGPROC_CONFIG_HPP__
# define __SIBR_IMGPROC_CONFIG_HPP__

# include "core/system/Config.hpp"
# include "core/system/Utils.hpp"

# ifdef SIBR_OS_WINDOWS
//// Export Macro (used for creating DLLs) ////
#  ifdef SIBR_STATIC_DEFINE
#    define SIBR_EXPORT
#    define SIBR_NO_EXPORT
#  else
#    ifndef SIBR_IMGPROC_EXPORT
#      ifdef SIBR_IMGPROC_EXPORTS
          /* We are building this library */
#        define SIBR_IMGPROC_EXPORT __declspec(dllexport)
#      else
          /* We are using this library */
#        define SIBR_IMGPROC_EXPORT __declspec(dllimport)
#      endif
#    endif
#    ifndef SIBR_NO_EXPORT
#      define SIBR_NO_EXPORT 
#    endif
#  endif
# else
# define SIBR_IMGPROC_EXPORT
# endif




#endif // __SIBR_IMGPROC_CONFIG_HPP__
