
#ifndef __SIBR_SCENE_CONFIG_HPP__
# define __SIBR_SCENE_CONFIG_HPP__

# include "core/graphics/Config.hpp"
# include <iomanip>


#ifdef SIBR_OS_WINDOWS
//// Export Macro (used for creating DLLs) ////
# ifdef SIBR_STATIC_DEFINE
#   define SIBR_EXPORT
#   define SIBR_NO_EXPORT
# else
#   ifndef SIBR_SCENE_EXPORT
#     ifdef SIBR_SCENE_EXPORTS
         /* We are building this library */
#       define SIBR_SCENE_EXPORT __declspec(dllexport)
#     else
         /* We are using this library */
#       define SIBR_SCENE_EXPORT __declspec(dllimport)
#     endif
#   endif
#   ifndef SIBR_NO_EXPORT
#     define SIBR_NO_EXPORT
#   endif
# endif
# else
#  define SIBR_SCENE_EXPORT
# endif


#endif // __SIBR_SCENE_CONFIG_HPP__
