#pragma once

#include "core/graphics/Config.hpp"

//// Export Macro (used for creating DLLs) ////
# ifdef SIBR_OS_WINDOWS
#  ifdef SIBR_STATIC_VIDEO_DEFINE
#    define SIBR_VIDEO_EXPORT
#    define SIBR_NO_VIDEO_EXPORT
#  else
#    ifndef SIBR_VIDEO_EXPORT
#      ifdef SIBR_VIDEO_EXPORTS
          /* We are building this library */
#        define SIBR_VIDEO_EXPORT __declspec(dllexport)
#      else
          /* We are using this library */
#        define SIBR_VIDEO_EXPORT __declspec(dllimport)
#      endif
#    endif
#    ifndef SIBR_NO_EXPORT
#      define SIBR_NO_EXPORT 
#    endif
#  endif
# else
#  define SIBR_VIDEO_EXPORT
# endif
