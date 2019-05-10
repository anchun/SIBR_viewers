
#ifndef __SIBR_RAYCASTER_CONFIG_HPP__
# define __SIBR_RAYCASTER_CONFIG_HPP__

# include <core/graphics/Config.hpp>


# ifdef SIBR_OS_WINDOWS
#  ifdef SIBR_STATIC_RAYCASTER_DEFINE
#    define SIBR_RAYCASTER_EXPORT
#    define SIBR_NO_RAYCASTER_EXPORT
#  else
#    ifndef SIBR_RAYCASTER_EXPORT
#      ifdef SIBR_RAYCASTER_EXPORTS
          /* We are building this library */
#        define SIBR_RAYCASTER_EXPORT __declspec(dllexport)
#      else
          /* We are using this library */
#        define SIBR_RAYCASTER_EXPORT __declspec(dllimport)
#      endif
#    endif
#  endif
# else
#  define SIBR_RAYCASTER_EXPORT
# endif


#endif // !__SIBR_RAYCASTER_CONFIG_HPP__
