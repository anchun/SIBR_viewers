## Included once for all sub project.
## It contain the whole cmake instructions to find necessary common dependencies.
## 3rdParty (provided by win3rdParty or from external packages) are then available in cmake sub projects.
##
## Do not include this file more than once but you can modify it to fit to your own project.
## So please, read it carefully because you can use on of these dependencies for your project or appen new one.
##
## As it is included after camke options, you can use conditional if(<CMAKE_PROJ_OPT>)/endif() to encapsulate your 3rdParty.
##
## Created/updated by jesnault (jerome.esnault@inria.fr) while last cmake version was 3.0.2
if(__dependencies_cmake_INCLUDED__)
	return()
else()
	set(__dependencies_cmake_INCLUDED__ ON)
endif()

## win3rdParty function allowing to auto check/download/update binaries dependencies for current windows compiler
## Please open this file in order to get more documentation and usage examples.
include(Win3rdParty)

include(sibr_library)

message("")
message(STATUS "[dependencies] Looking for necessary dependencies:")
Win3rdPartyGlobalCacheAction()



##
## All cmake instructions not inside if BUILD_* condition are common to all projects
##

find_package(OpenGL REQUIRED) ## Not really requiered here since we use the OpenGL from Qt5 (see libls-mini)

if(USE_RENDERER_API_GLUT)

    ############
    ## Find GLUT
    ############
    # message(STATUS "before calling win3rdParty GLUT")

    win3rdParty(GLUT #VERBOSE ON
        MSVC11 "win3rdParty/MSVC11/GLUT" "https://gforge.inria.fr/frs/download.php/file/34358/freeglut-2.8.1.7z"
        MSVC12 "win3rdParty/MSVC11/GLUT" "https://gforge.inria.fr/frs/download.php/file/34358/freeglut-2.8.1.7z"
        MSVC14 "win3rdParty/MSVC14/GLUT" "https://gforge.inria.fr/frs/download.php/file/36893/freeglut-2.8.1.7z"    # using recompiled version of freeglut
        MULTI_SET
            CHECK_CACHED_VAR GLUT_ROOT_PATH     PATH    "freeglut-2.8.1"
            CHECK_CACHED_VAR OPENGL_LIBRARY_DIR PATH    "freeglut-2.8.1/${LIB_BUILT_DIR}/Release"
            CHECK_CACHED_VAR GLUT_INCLUDE_DIR	PATH    "freeglut-2.8.1/include"
            CHECK_CACHED_VAR GLUT_glut_LIBRARY	STRING  "freeglut-2.8.1/${LIB_BUILT_DIR}/Release/freeglut.lib"
    )
    FIND_PACKAGE(GLUT REQUIRED)
    IF(GLUT_FOUND)
        INCLUDE_DIRECTORIES(${GLUT_INCLUDE_DIR})
    ELSE(GLUT_FOUND)
        MESSAGE("GLUT not found. Set GLUT_ROOT_PATH to base directory of GLUT.")
    ENDIF(GLUT_FOUND)


    ############
    ## Find GLEW
    ############
    if (MSVC11 OR MSVC12)
        set(glew_multiset_arguments 
                CHECK_CACHED_VAR GLEW_INCLUDE_DIR	    PATH "glew-1.10.0/include" DOC "default empty doc"
                CHECK_CACHED_VAR GLEW_LIBRARIES         STRING LIST "debug;glew-1.10.0/${LIB_BUILT_DIR}/glew32d.lib;optimized;glew-1.10.0/${LIB_BUILT_DIR}/glew32.lib" DOC "default empty doc"
            )
    elseif (MSVC14)
        set(glew_multiset_arguments 
                CHECK_CACHED_VAR GLEW_INCLUDE_DIR	    PATH "glew-2.0.0/include" DOC "default empty doc"
                CHECK_CACHED_VAR GLEW_LIBRARIES         STRING LIST "debug;glew-2.0.0/${LIB_BUILT_DIR}/glew32d.lib;optimized;glew-2.0.0/${LIB_BUILT_DIR}/glew32.lib" DOC "default empty doc"
            )
    else ()
        message("There is no provided GLEW library for your version of MSVC")
    endif()
    win3rdParty(GLEW #VERBOSE ON
        MSVC11 "win3rdParty/MSVC11/GLEW" "https://gforge.inria.fr/frs/download.php/file/34357/glew-1.10.0.7z"
        MSVC12 "win3rdParty/MSVC11/GLEW" "https://gforge.inria.fr/frs/download.php/file/34357/glew-1.10.0.7z"
        MSVC14 "win3rdParty/MSVC14/GLEW" "https://gforge.inria.fr/frs/download.php/file/37620/glew-2.0.0.7z"        # using recompiled version of glew
        MULTI_SET ${glew_multiset_arguments}
    )
    FIND_PACKAGE(GLEW REQUIRED)
    IF(GLEW_FOUND)
        INCLUDE_DIRECTORIES(${GLEW_INCLUDE_DIR})
    ELSE(GLEW_FOUND)
        MESSAGE("GLEW not found. Set GLEW_DIR to base directory of GLEW.")
    ENDIF(GLEW_FOUND)

elseif(USE_RENDERER_API_QT)

    ############
    ## Find Qt5
    ############
    if(ARCHI_BUILT_DIR MATCHES "x64") ## ARCHI_BUILT_DIR is set in top root CMakeLists.txt
        win3rdParty(Qt5 TIMEOUT 800 DEFAULT_USE OFF #VERBOSE ON
                MSVC11 "win3rdParty/MSVC11/Qt5" "https://gforge.inria.fr/frs/download.php/file/34393/Qt5.4-msvc11-x64.7z"
                MSVC12 "win3rdParty/MSVC11/Qt5" "https://gforge.inria.fr/frs/download.php/file/34393/Qt5.4-msvc11-x64.7z"
                SET CHECK_CACHED_VAR Qt5_DIR PATH "Qt5.4-msvc11-x64/lib/cmake/Qt5"
        )
    elseif(ARCHI_BUILT_DIR MATCHES "x86")
        win3rdParty(Qt5 TIMEOUT 800 DEFAULT_USE OFF #VERBOSE ON
                MSVC11 "win3rdParty/MSVC11/Qt5" "https://gforge.inria.fr/frs/download.php/file/34394/Qt5.4-mscv11-x86.7z"
                MSVC12 "win3rdParty/MSVC11/Qt5" "https://gforge.inria.fr/frs/download.php/file/34393/Qt5.4-msvc11-x64.7z"
                SET CHECK_CACHED_VAR Qt5_DIR PATH "Qt5.4-mscv11-x86/lib/cmake/Qt5"
        )
    endif()

    ## WORK AROUND with windows : try to auto find and set Qt5_DIR
    if(WIN32)
        file(GLOB qt5versionPathList "C:/Qt/Qt5.*")
        if(NOT Qt5_DIR AND qt5versionPathList)
            list(LENGTH qt5versionPathList qt5versionPathListCount)
            if(${qt5versionPathListCount} GREATER "1")
                message("Many Qt5 version auto detected (check manually the right one with Qt5_DIR cmake variable).")
            endif()
            foreach(qt5versionPath ${qt5versionPathList})
                ## go deep to look for any qt5 install dir (sdk include/lib dirs)
                file(GLOB qt5versionSubPathList "${qt5versionPath}/5.*")
                file(GLOB qt5versionSubPathList "${qt5versionSubPathList}/*")
                if(qt5versionSubPathList)
                    foreach(qt5versionSubPath ${qt5versionSubPathList})
                        get_filename_component(redistArch ${qt5versionSubPath} NAME)
                        string(REGEX MATCH 	"[A-Za-z_0-9-]+64[A-Za-z_0-9-]+" 64archMatched ${redistArch})
                        if(64archMatched)
                            set(qtArch x64)
                        else()
                            set(qtArch x86)
                        endif()
                        message("Plausible Qt5 instllation dir [${qtArch}] : ${qt5versionSubPath}")
                        if(CMAKE_SIZEOF_VOID_P MATCHES "8")
                            if("${qtArch}" MATCHES "x64")
                                set(Qt5_DIR "${qt5versionSubPath}/lib/cmake/Qt5") ## choose last one
                            endif()
                        else()
                            if("${qtArch}" MATCHES "x86")
                                set(Qt5_DIR "${qt5versionSubPath}/lib/cmake/Qt5") ## choose last one
                            endif()
                        endif()
                    endforeach()
                endif()
            endforeach()
        endif()
    endif()
    set(Qt5_DIR ${Qt5_DIR} CACHE PATH "Path to <Qt5 installation>/lib/cmake/Qt5")

    set(QtComponents OpenGL Widgets)
    if(BUILD_IS) ## for is submodule Qt5 dependencies
        list(APPEND QtComponents Network Qml Quick)
    endif()

    ## WORK AROUND: QT5 5.2.1 win32 OpenGL problem to find glu32... we need WINSDK to let qt find it
    list(FIND QtComponents OpenGL haveOpenGLComponent)
    if(NOT "${haveOpenGLComponent}" MATCHES "-1" AND WIN32)
        find_package(WindowsSDK QUIET)
        if(WindowsSDK_FOUND)
            message(STATUS "WindowsSDK ${WindowsSDK_VERSION} found for QOpenGL component using Qt5 < 5.3")
            list(APPEND CMAKE_LIBRARY_PATH ${WindowsSDK_LIBRARY_DIRS})
        else()
            message(WARNING "You need Windows SDK to let Qt5 find OpenGL glu32.")
        endif()
    endif()

    set(CMAKE_AUTOMOC ON) # Instruct CMake to run moc automatically when needed.
    find_package(Qt5 COMPONENTS ${QtComponents} REQUIRED) ## then you can use : qt5_use_modules(<target> <component> <...>)
    include(Qt5ImportedLocation) ## function mainly used to get binaries location for installation under windows (*.dll)

    if(NOT Qt5_FOUND)
        message(SEND_ERROR "Qt5 not found, please set Qt5_DIR to <Qt5 installation>/lib/cmake/Qt5")
    elseif(WIN32 AND "${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}.${QT_VERSION_PATCH}" MATCHES "5.2.1")
        message(WARNING "Be careful, there were bugs on 5.2.1 win32 version : http://stackoverflow.com/questions/14008737/qt5-qconfig-h-weird")
    endif()

else()
    message(WARNING "USE_RENDERER_API is not Qt nor GLUT : USE_RENDERER_API=${USE_RENDERER_API} you need to choose a correct one.")
endif()


# ############################################################
# ## Find OpenGL helper functions from mini LibSL library
# ############################################################
# find_package(LIBSLMINI)
# if(LIBSLMINI_FOUND)
# 	if(USE_RENDERER_API_QT)
# 	    include_directories(${LIBSLMINI_QT_HEADERS_ONLY_DIR})
# 	elseif(USE_RENDERER_API_GLUT)
# 	    include_directories(${LIBSLMINI_GL_HEADERS_ONLY_DIR})
#     else()
#         message(WARNING "no include_directories of LIBSLMINI was done. Check USE_RENDERER_API* vars.")
# 	endif()
# else()
# 	message(SEND_ERROR "LIBSLMINI is not found")
# 	message("LIBSLMINI_GL_HEADERS_ONLY_DIR=${LIBSLMINI_GL_HEADERS_ONLY_DIR}")
# 	message("LIBSLMINI_QT_HEADERS_ONLY_DIR=${LIBSLMINI_QT_HEADERS_ONLY_DIR}")
# endif()


##############
## Find ASSIMP
##############
## TODO: load as plugin by IBR_common only if requiered
## Find ASSIMP (in order to load mesh)
    if (MSVC11 OR MSVC12)
        set(assimp_set_arguments 
            CHECK_CACHED_VAR ASSIMP_DIR PATH "Assimp_3.1_fix"
        )
    elseif (MSVC14)
        set(assimp_set_arguments 
            CHECK_CACHED_VAR ASSIMP_DIR PATH "Assimp-3.3.1"
        )
    else ()
        message("There is no provided ASSIMP library for your version of MSVC")
    endif()

win3rdParty(ASSIMP #VERBOSE ON
        MSVC11 "win3rdParty/MSVC11/ASSIMP" "https://gforge.inria.fr/frs/download.php/file/35212/Assimp_3.1_fix.7z"
        MSVC12 "win3rdParty/MSVC12/ASSIMP" "https://gforge.inria.fr/frs/download.php/file/35212/Assimp_3.1_fix.7z"
        MSVC14 "win3rdParty/MSVC14/ASSIMP" "https://gforge.inria.fr/frs/download.php/file/36955/Assimp-3.3.1.7z"
        SET ${assimp_set_arguments}
)
find_package(ASSIMP REQUIRED)
include_directories(${ASSIMP_INCLUDE_DIR})

################
## Find FFMPEG
################
if(BUILD_VBR OR BUILD_IBR_DYNAMIC) ## ffmpeg only needed with vbr projects
	win3rdParty(FFMPEG
		MSVC11 "win3rdParty/MSVC11/ffmpeg" "https://gforge.inria.fr/frs/download.php/file/34916/ffmpeg.zip"
        MSVC12 "win3rdParty/MSVC12/ffmpeg" "https://gforge.inria.fr/frs/download.php/file/34916/ffmpeg.zip"
        MSVC14 "win3rdParty/MSVC14/ffmpeg" "https://gforge.inria.fr/frs/download.php/file/37657/ffmpeg-4.0.2-win64-win3rdParty.7z"
        SET CHECK_CACHED_VAR FFMPEG_DIR PATH ${FFMPEG_WIN3RDPARTY_DIR}
	)
	find_package(FFmpeg QUIET)
	if(FFMPEG_FOUND)
	  include_directories(${FFMPEG_INCLUDE_DIR})
	  message("FFmpeg was found in : ${FFMPEG_INCLUDE_DIR}")
	endif()
endif()


###################
## Find LibRaw
###################
sibr_addlibrary(
	NAME LibRaw
	MSVC11 "https://gforge.inria.fr/frs/download.php/file/35595/libraw-0.17.1.7z"
    MSVC14 "https://gforge.inria.fr/frs/download.php/file/35595/libraw-0.17.1.7z"               # TODO SV: provide a valid version if required
	REQUIREDFOR BUILD_MVIIR
)

###################
## Find PANO13
###################
sibr_addlibrary(
    NAME pano13
    MSVC11 "https://gforge.inria.fr/frs/download.php/file/35874/pano13.7z"
    MSVC14 "https://gforge.inria.fr/frs/download.php/file/35874/pano13.7z"                      # TODO SV: provide a valid version if required
    REQUIREDFOR BUILD_UTILITIES_GSVSAMPLER
)

###################
## Find embree2
###################
sibr_addlibrary(
    NAME embree2
    MSVC11 "https://gforge.inria.fr/frs/download.php/file/35964/embree2.7.0.x64.windows.7z"
    MSVC14 "https://gforge.inria.fr/frs/download.php/file/35964/embree2.7.0.x64.windows.7z"     # TODO SV: provide a valid version if required
    REQUIREDFOR BUILD_SIBR
)

###################
## Find OpenImageIO
###################
if(BUILD_UTILITIES_OIIO_EXAMPLE) ## will need boost (thread system chrono) also (see above)
    win3rdParty(OPENIMAGEIO #VERBOSE ON
            MSVC11 "win3rdParty/MSVC11/OpenImageIO" "https://gforge.inria.fr/frs/download.php/file/34749/OpenImageIO-1.6.7z"
            #MSVC12 "win3rdParty/MSVC12/OpenImageIO" "TODO"
            MSVC14 "win3rdParty/MSVC14/OpenImageIO" "https://gforge.inria.fr/frs/download.php/file/34749/OpenImageIO-1.6.7z"    # TODO SV: provide a valid version if required
            MULTI_SET
                CHECK_CACHED_VAR OPENIMAGEIO_DIR    PATH "OpenImageIO-1.6"
                CHECK_CACHED_VAR ILMBASE_HOME       PATH "OpenImageIO-1.6/external/ilmbase-2.2.0"
                CHECK_CACHED_VAR OPENEXR_HOME       PATH "OpenImageIO-1.6/external/openexr-2.2.0"
    )
    find_package(OpenImageIO)
    if(NOT OpenImageIO_FOUND)
        message(WARNING "OpenImageIO NOT FOUND!!")
    else()
        include_directories(${OPENIMAGEIO_INCLUDE_DIR})
    endif()

    ## IlmBase (needed by oiio)
    find_package(IlmBase)
    if(ILMBASE_FOUND)
        include_directories ("${ILMBASE_INCLUDE_DIR}")
        include_directories ("${ILMBASE_INCLUDE_DIR}/OpenEXR")
        list(APPEND OPENIMAGEIO_LIBRARIES ${ILMBASE_LIBRARIES})
    else()
        message(WARNING "IlmBase NOT FOUND, needed with OpenImageIO!!")
    endif()

    ## OpenEXR (needed by oiio)
    find_package(OpenEXR)
    if(OPENEXR_FOUND)
        if (EXISTS "${OPENEXR_INCLUDE_DIR}/OpenEXR/ImfMultiPartInputFile.h")
            add_definitions (-DUSE_OPENEXR_VERSION2=1)
        endif()
        include_directories ("${OPENEXR_INCLUDE_DIR}")
        include_directories ("${OPENEXR_INCLUDE_DIR}/OpenEXR")
        list(APPEND OPENIMAGEIO_LIBRARIES ${OPENEXR_LIBRARIES})
    else()
        message(WARNING "OpenEXR NOT FOUND, needed with OpenImageIO!!")
    endif()

    if(OpenImageIO_FOUND)
        list(REMOVE_DUPLICATES OPENIMAGEIO_LIBRARIES)
        message(STATUS "OPENIMAGEIO_LIBRARIES=${OPENIMAGEIO_LIBRARIES}")
    endif()
endif()

################
## Find AntTweakBar
################
if(BUILD_VBR) ## AntTweakBar only needed with vbr projects
	win3rdParty(AntTweakBar
		MSVC11 "win3rdParty/MSVC11/AntTweakBar" "https://gforge.inria.fr/frs/download.php/file/34915/AntTweakBar.zip"
        MSVC12 "win3rdParty/MSVC12/AntTweakBar" "https://gforge.inria.fr/frs/download.php/file/34915/AntTweakBar.zip"
        MSVC14 "win3rdParty/MSVC14/AntTweakBar" "https://gforge.inria.fr/frs/download.php/file/34915/AntTweakBar.zip"           # TODO SV: provide a valid version if required
        SET CHECK_CACHED_VAR AntTweakBar_DIR PATH ${AntTweakBar_WIN3RDPARTY_DIR}
	)
	find_package(AntTweakBar QUIET)
	if(ANTTWEAKBAR_FOUND)
	  include_directories(${AntTweakBar_INCLUDE_DIR})
	  message("AntTweakBar was found in : ${AntTweakBar_INCLUDE_DIR}")
	endif()
endif()

###########
## Find GLM
###########
if(BUILD_VBR)
	win3rdParty(GLM #VERBOSE ON
	   MSVC11 "win3rdParty/MSVC11/GLM" "https://gforge.inria.fr/frs/download.php/file/34917/glm.zip"
	   MSVC12 "win3rdParty/MSVC12/GLM" "https://gforge.inria.fr/frs/download.php/file/34917/glm.zip"
	   MSVC14 "win3rdParty/MSVC14/GLM" "https://gforge.inria.fr/frs/download.php/file/34917/glm.zip"    # TODO SV: provide a valid version if required
	   SET CHECK_CACHED_VAR GLM_DIR PATH ${GLM_WIN3RDPARTY_DIR}
	)
	find_package(GLM REQUIRED)
	if(GLM_FOUND)
		include_directories(${GLM_INCLUDE_DIR})
		message("GLM was found in : ${GLM_INCLUDE_DIR}")
	else()
		message(SEND_ERROR "GLM not found.")
	endif()
endif()


##############
## Find zlib
##############
win3rdParty(ZLIB #VERBOSE ON
        MSVC11 "win3rdParty/MSVC11/zlib" "https://gforge.inria.fr/frs/download.php/file/35189/zlib-1.2.8.7z"
        MSVC12 "win3rdParty/MSVC12/zlib" "https://gforge.inria.fr/frs/download.php/file/35189/zlib-1.2.8.7z"
        MSVC14 "win3rdParty/MSVC14/zlib" "https://gforge.inria.fr/frs/download.php/file/35189/zlib-1.2.8.7z"    # TODO SV: provide a valid version if required
        SET CHECK_CACHED_VAR ZLIB_ROOT PATH "zlib-1.2.8"
)
find_package(ZLIB REQUIRED)
include_directories(${ZLIB_INCLUDE_DIR})

##############
## Find jpeg
##############
win3rdParty(JPEG #VERBOSE ON
        MSVC11 "win3rdParty/MSVC11/jpeg" "https://gforge.inria.fr/frs/download.php/file/35406/jpeg-9a-2.7z"
        MSVC12 "win3rdParty/MSVC12/jpeg" "https://gforge.inria.fr/frs/download.php/file/35406/jpeg-9a-2.7z"
        MSVC14 "win3rdParty/MSVC14/jpeg" "https://gforge.inria.fr/frs/download.php/file/35406/jpeg-9a-2.7z"    # TODO SV: provide a valid version if required
        SET CHECK_CACHED_VAR JPEG_DIR PATH "jpeg-9a"
)
find_package(JPEG REQUIRED)
include_directories(${JPEG_INCLUDE_DIR})

###################
## Find eigen3
###################
sibr_addlibrary(
	NAME eigen3
	#MSVC11 "https://gforge.inria.fr/frs/download.php/file/36127/eigen-eigen-dc6cfdf9bcec.7z"
	#MSVC14 "https://gforge.inria.fr/frs/download.php/file/36127/eigen-eigen-dc6cfdf9bcec.7z"    # TODO SV: provide a valid version if required
    
    MSVC11 "https://gforge.inria.fr/frs/download.php/file/37880/eigen3.7z"
    MSVC14 "https://gforge.inria.fr/frs/download.php/file/37880/eigen3.7z"

    REQUIREDFOR BUILD_SIBR
)
include_directories(/usr/include/eigen3)
#include_directories(/user/sbonoper/local/eigen3/eigen-eigen-dc6cfdf9bcec)


# ###################
# ## Find PNG
# ###################
# sibr_addlibrary(
	# NAME libpng
	# MSVC11 "https://gforge.inria.fr/frs/download.php/file/35187/libpng-1.7.0.b54.7z"
    # REQUIREDFOR BUILD_SIBR
# )
# find_package(PNG REQUIRED)
# include_directories(${PNG_INCLUDE_DIR})

# ###################
# ## Find tiff
# ###################
# sibr_addlibrary(
	# NAME tiff
	# MSVC11 "https://gforge.inria.fr/frs/download.php/file/35408/tiff-4.0.6-2.7z"
    # REQUIREDFOR BUILD_SIBR
# )
# find_package(TIFF REQUIRED)
# include_directories(${TIFF_INCLUDE_DIR})


##############
## Find PNG
##############
win3rdParty(PNG #VERBOSE ON
        MSVC11 "win3rdParty/MSVC11/libpng" "https://gforge.inria.fr/frs/download.php/file/35187/libpng-1.7.0.b54.7z"
        MSVC12 "win3rdParty/MSVC12/libpng" "https://gforge.inria.fr/frs/download.php/file/35187/libpng-1.7.0.b54.7z"
        MSVC14 "win3rdParty/MSVC14/libpng" "https://gforge.inria.fr/frs/download.php/file/35187/libpng-1.7.0.b54.7z"    # TODO SV: provide a valid version if required
        SET CHECK_CACHED_VAR PNG_DIR PATH "libpng-1.7.0.b54"
)
find_package(PNG REQUIRED)
include_directories(${PNG_INCLUDE_DIR})

##############
## Find tiff
##############
win3rdParty(TIFF #VERBOSE ON
        MSVC11 "win3rdParty/MSVC11/tiff" "https://gforge.inria.fr/frs/download.php/file/35408/tiff-4.0.6-2.7z"
        MSVC12 "win3rdParty/MSVC12/tiff" "https://gforge.inria.fr/frs/download.php/file/35408/tiff-4.0.6-2.7z"
        MSVC14 "win3rdParty/MSVC14/tiff" "https://gforge.inria.fr/frs/download.php/file/35408/tiff-4.0.6-2.7z"    # TODO SV: provide a valid version if required
        SET CHECK_CACHED_VAR TIFF_DIR PATH "tiff-4.0.6"
)
find_package(TIFF REQUIRED)
include_directories(${TIFF_INCLUDE_DIR})

##############
## Find Embree (v2)
##############
if (BUILD_MVIIR)
	win3rdParty(EMBREE #VERBOSE ON
        	MSVC11 "win3rdParty/MSVC11/embree" "https://gforge.inria.fr/frs/download.php/file/35275/embree-2.7.0.x64.windows.7z"
        	MSVC12 "win3rdParty/MSVC11/embree" "https://gforge.inria.fr/frs/download.php/file/35275/embree-2.7.0.x64.windows.7z"
        	MSVC14 "win3rdParty/MSVC14/embree" "https://gforge.inria.fr/frs/download.php/file/35275/embree-2.7.0.x64.windows.7z"    # TODO SV: provide a valid version if required
        	SET CHECK_CACHED_VAR EMBREE_DIR PATH "embree-2.7.0.x64.windows"
			)
	find_package(Embree REQUIRED)
	include_directories(${EMBREE_INCLUDE_DIR})
endif()

##############
if (BUILD_PREPROCESS)

	# libigl
    # message("libigl win3rdparty dir: ${Libigl_WIN3RDPARTY_DIR}")
    # set(Libigl_WIN3RDPARTY_DIR "libigl-master")
	win3rdParty(Libigl
		MSVC11 "win3rdParty/MSVC11/libigl" "https://gforge.inria.fr/frs/download.php/file/35892/libigl.zip"
        MSVC12 "win3rdParty/MSVC12/libigl" "https://gforge.inria.fr/frs/download.php/file/35892/libigl.zip"
        MSVC14 "win3rdParty/MSVC14/libigl" "https://gforge.inria.fr/frs/download.php/file/35892/libigl.zip"    # TODO SV: provide a valid version if required
        SET CHECK_CACHED_VAR Libigl_DIR PATH ${Libigl_WIN3RDPARTY_DIR}
        # SET CHECK_CACHED_VAR Libigl_DIR PATH "libigl"
	)
	find_package(Libigl REQUIRED)

	include_directories(${Libigl_INCLUDE_DIR})
    message("libigl was found in : ${Libigl_INCLUDE_DIR}")

	## Find gflags
	## Levmar only needed with vbr projects
    # message("gflags win3rdparty dir: ${GFLAGS_WIN3RDPARTY_DIR}")
	win3rdParty(GFLAGS
		MSVC11 "win3rdParty/MSVC11/gflags" "https://gforge.inria.fr/frs/download.php/latestfile/4830/gflags.zip"
        MSVC12 "win3rdParty/MSVC12/gflags" "https://gforge.inria.fr/frs/download.php/latestfile/4830/gflags.zip"
        MSVC14 "win3rdParty/MSVC14/gflags" "https://gforge.inria.fr/frs/download.php/latestfile/4830/gflags.zip"    # TODO SV: provide a valid version if required
        SET CHECK_CACHED_VAR GFLAGS_DIR PATH ${GFLAGS_WIN3RDPARTY_DIR}
        # SET CHECK_CACHED_VAR GFLAGS_DIR PATH "gflags"
	)
	find_package(gflags QUIET)
	include_directories(${GFLAGS_INCLUDE_DIR})
	message("GFLAGS was found in : ${GFLAGS_INCLUDE_DIR}")

	## Find glog
	## Levmar only needed with vbr projects
	win3rdParty(GLOG
		MSVC11 "win3rdParty/MSVC11/glog" "https://gforge.inria.fr/frs/download.php/latestfile/4830/glog.zip"
        MSVC12 "win3rdParty/MSVC12/glog" "https://gforge.inria.fr/frs/download.php/latestfile/4830/glog.zip"
        MSVC14 "win3rdParty/MSVC14/glog" "https://gforge.inria.fr/frs/download.php/latestfile/4830/glog.zip"    # TODO SV: provide a valid version if required
        SET CHECK_CACHED_VAR GLOG_DIR PATH ${GLOG_WIN3RDPARTY_DIR}
        # SET CHECK_CACHED_VAR GLOG_DIR PATH "glog"
	)
	find_package(glog QUIET)
	include_directories(${GLOG_INCLUDE_DIR})
	message("GLOG was found in : ${GLOG_INCLUDE_DIR}")

	## Find ceres
	## Levmar only needed with vbr projects
    message("gflags win3rdparty dir: ${GFLAGS_WIN3RDPARTY_DIR}")
	win3rdParty(CERES
		MSVC11 "win3rdParty/MSVC11/ceres" "https://gforge.inria.fr/frs/download.php/latestfile/4830/ceres.zip"
        MSVC12 "win3rdParty/MSVC12/ceres" "https://gforge.inria.fr/frs/download.php/latestfile/4830/ceres.zip"
        MSVC14 "win3rdParty/MSVC14/ceres" "https://gforge.inria.fr/frs/download.php/latestfile/4830/ceres.zip"    # TODO SV: provide a valid version if required
        SET CHECK_CACHED_VAR CERES_DIR PATH ${CERES_WIN3RDPARTY_DIR}
        # SET CHECK_CACHED_VAR CERES_DIR PATH "ceres"
	)
	find_package(ceres QUIET)
	include_directories(${CERES_INCLUDE_DIR})
	message("CERES was found in : ${CERES_INCLUDE_DIR}")

endif()



# ##############
# ## Find Eigen (v3)
# ##############
# if (1)#BUILD_MVIIR)
# 	if (DEFINED ENV{EIGEN3_INCLUDE_DIR})
# 		set(EIGEN3_INCLUDE_DIR "$ENV{EIGEN3_INCLUDE_DIR}" CACHE PATH "EIGEN3 include directory")
# 		message(STATUS "Using EIGEN3_INCLUDE_DIR from environment variable (=$ENV{EIGEN3_INCLUDE_DIR})")
# 	else()
# 		win3rdParty(EIGEN3 #VERBOSE ON
#         		MSVC11 "win3rdParty/MSVC11/eigen" "https://gforge.inria.fr/frs/download.php/file/35227/eigen-eigen-c58038c56923.7z"
#         		MSVC12 "win3rdParty/MSVC11/eigen" "https://gforge.inria.fr/frs/download.php/file/35227/eigen-eigen-c58038c56923.7z"
#         		SET CHECK_CACHED_VAR EIGEN3_DIR PATH "eigen-eigen-c58038c56923"
# 					)
# 			find_package(Eigen REQUIRED)
# 	endif()
# 	include_directories(${EIGEN3_INCLUDE_DIR})
# endif()
add_definitions(-DEIGEN_INITIALIZE_MATRICES_BY_ZERO)



############################
## Find 3DMouse SDK (optional)
############################
option(USE_SPACEWARE "Use 3D Mouse interaction" OFF)
if(USE_SPACEWARE)
    win3rdParty(SPACEWARE #VERBOSE ON
        MSVC11 "win3rdParty/MSVC11/SPACEWARE" "https://gforge.inria.fr/frs/download.php/file/34362/3DxWareSDK.7z"
        MSVC12 "win3rdParty/MSVC11/SPACEWARE" "https://gforge.inria.fr/frs/download.php/file/34362/3DxWareSDK.7z"
        MSVC14 "win3rdParty/MSVC14/SPACEWARE" "https://gforge.inria.fr/frs/download.php/file/34362/3DxWareSDK.7z"    # TODO SV: provide a valid version if required
        SET CHECK_CACHED_VAR SPACEWARE_DIR PATH "3DxWareSDK"
    )
	find_package(SPACEWARE QUIET)
	if(SPACEWARE_FOUND)
		include_directories(${SPACEWARE_INCLUDE_DIR})
		link_directories(${SPACEWARE_LIBRARY_DIR})
		add_definitions(-DHAS_SPACEWARE)
	else()
		message("SPACEWARE (3DxWareSDK for 3D mouse) was not found and the associated interactor should be disabled.")
		set(SPACEWARE_LIBRARIES )
	endif()
    message(WARNING "SPACEWARE may cause troubles in runtime debug mode (*.exe won't run in debug)...")
endif()


#################################
## Find LeapMotion SDK (optional)
#################################
option(USE_LEAPMOTION "Use hands tracker leapMotion device" OFF)
if(USE_LEAPMOTION)
    win3rdParty(LeapMotion #VERBOSE ON
        MSVC11 "win3rdParty/MSVC11/LeapMotion" "https://gforge.inria.fr/frs/download.php/file/34375/LeapSDK_1.2.0.7z"
        MSVC12 "win3rdParty/MSVC11/LeapMotion" "https://gforge.inria.fr/frs/download.php/file/34375/LeapSDK_1.2.0.7z"
        MSVC14 "win3rdParty/MSVC14/LeapMotion" "https://gforge.inria.fr/frs/download.php/file/34375/LeapSDK_1.2.0.7z"    # TODO SV: provide a valid version if required
        SET CHECK_CACHED_VAR LEAP_DIR PATH "LeapSDK_1.2.0"
    )
    find_package(LeapMotion QUIET)
    IF(LEAP_FOUND)
        INCLUDE_DIRECTORIES(${LEAP_INCLUDE_DIR})
        add_definitions(-DHAS_LEAP_MOTION)
    else()
        set(LEAP_LIBRARIES )
    endif()
endif()


####################
## Find SuiteSparse
####################
if(BUILD_IBR_HYBRID OR BUILD_IBR_SPIXELWARP OR BUILD_IBR_ULRSKY)
     win3rdParty(SuiteSparse #VERBOSE ON
         MSVC11 "win3rdParty/MSVC11/SuiteSparse" "https://gforge.inria.fr/frs/download.php/file/34361/SuiteSparse-4.2.1.7z"
         MSVC12 "win3rdParty/MSVC11/SuiteSparse" "https://gforge.inria.fr/frs/download.php/file/34361/SuiteSparse-4.2.1.7z"
         MSVC14 "win3rdParty/MSVC14/SuiteSparse" "https://gforge.inria.fr/frs/download.php/file/34361/SuiteSparse-4.2.1.7z"    # TODO SV: provide a valid version if required
         MULTI_SET
             CHECK_CACHED_VAR SuiteSparse_DIR             PATH "SuiteSparse-4.2.1"
             CHECK_CACHED_VAR SuiteSparse_USE_LAPACK_BLAS BOOL ON
     )
 	find_package(SuiteSparse QUIET NO_MODULE)
 	if(NOT SuiteSparse_FOUND)
 		SET(SuiteSparse_VERBOSE ON)
 		find_package(SuiteSparse REQUIRED COMPONENTS amd camd colamd ccolamd cholmod)
 		if(SuiteSparse_FOUND)
 			include_directories(${SuiteSparse_INCLUDE_DIRS})
 		else()
            message(SEND_ERROR "SuiteSparse not found.")
            set(BUILD_IBR_HYBRID 		OFF)
            set(BUILD_IBR_SPIXELWARP 	OFF)
            set(BUILD_IBR_ULRSKY        OFF)
            message("BUILD_IBR_HYBRID		= ${BUILD_IBR_HYBRID}")
            message("BUILD_IBR_SPIXELWARP	= ${BUILD_IBR_SPIXELWARP}")
            message("BUILD_IBR_ULRSKY	    = ${BUILD_IBR_ULRSKY}")
 		endif()
 	else()
 		message(STATUS "Find SuiteSparse: INCLUDE(${USE_SuiteSparse})")
 		include(${USE_SuiteSparse})
 	endif()
 endif()


##############
## Find CGAL
##############
if (MSVC11 OR MSVC12)
    set(cgal_multiset_arguments 
        CHECK_CACHED_VAR CGAL_INCLUDE_DIR   PATH "CGAL-4.3/include"
        #CHECK_CACHED_VAR CGAL_LIBRARIES	    STRING LIST "debug;CGAL-4.3/${LIB_BUILT_DIR}/CGAL-${CGAL_WIN3RDPARTY_VCID}-mt-gd-4.3.lib;optimized;CGAL-4.3/${LIB_BUILT_DIR}/CGAL-${CGAL_WIN3RDPARTY_VCID}-mt-4.3.lib"
        CHECK_CACHED_VAR CGAL_LIBRARIES	    STRING LIST "debug;CGAL-4.3/${LIB_BUILT_DIR}/CGAL-vc110-mt-gd-4.3.lib;optimized;CGAL-4.3/${LIB_BUILT_DIR}/CGAL-vc110-mt-4.3.lib"
    )
elseif (MSVC14)
    set(cgal_multiset_arguments 
        CHECK_CACHED_VAR CGAL_INCLUDE_DIR   PATH "CGAL-4.10/include"
		CHECK_CACHED_VAR CGAL_LIBRARIES	    STRING LIST "debug;CGAL-4.10/${LIB_BUILT_DIR}/CGAL-vc140-mt-gd-4.10.lib;optimized;CGAL-4.10/${LIB_BUILT_DIR}/CGAL-vc140-mt-4.10.lib"
        )
else ()
    message("There is no provided CGAL library for your version of MSVC")
endif()
if(BUILD_IBR_HYBRID OR BUILD_IBR_SPIXELWARP OR BUILD_IBR_ULRSKY OR BUILD_PREPROCESS OR BUILD_COMPARE)
    win3rdParty(CGAL VCID #VERBOSE ON
        MSVC11 "win3rdParty/MSVC11/CGAL" "https://gforge.inria.fr/frs/download.php/file/34359/CGAL-4.3.7z"
        MSVC12 "win3rdParty/MSVC11/CGAL" "https://gforge.inria.fr/frs/download.php/file/34359/CGAL-4.3.7z"
        MSVC14 "win3rdParty/MSVC14/CGAL" "https://gforge.inria.fr/frs/download.php/file/36902/CGAL-4.10.7z"    # cgal compatible with msvc14
    )
    win3rdParty(CGAL MULTI_SET ${cgal_multiset_arguments})
	find_package(CGAL QUIET NO_MODULE NO_CMAKE_BUILDS_PATH)
	if(NOT CGAL_FOUND)
		find_package(CGAL REQUIRED )
		include_directories(${CGAL_INCLUDE_DIR})
	else()
		if(CGAL_USE_FILE) ## FOUND from the standard official install dir
			include(${CGAL_USE_FILE})
		else()
			message(SEND_ERROR "CGAL not found.")
			set(BUILD_IBR_HYBRID 		OFF)
			set(BUILD_IBR_SPIXELWARP 	OFF)
			message("BUILD_IBR_HYBRID		= ${BUILD_IBR_HYBRID}")
			message("BUILD_IBR_SPIXELWARP	= ${BUILD_IBR_SPIXELWARP}")
		endif()
	endif()
endif()


#############
## Find Boost
#############
if(1)
    set(Boost_NEEDED_COMPONENTS )
    if(CGAL_FOUND)
        list(APPEND Boost_NEEDED_COMPONENTS thread system chrono)
    endif()
    if(BUILD_IBR_CROWD)
        list(APPEND Boost_NEEDED_COMPONENTS thread system chrono serialization filesystem)
    endif()
    if(BUILD_IBR_UTILITIES_MVSEG)
        list(APPEND Boost_NEEDED_COMPONENTS log thread system chrono filesystem date_time)
    endif()
    if(BUILD_UTILITIES_OIIO_EXAMPLE)
        list(APPEND Boost_NEEDED_COMPONENTS thread system chrono)
    endif()
	if(BUILD_MVIIR)
		list(APPEND Boost_NEEDED_COMPONENTS log thread system chrono filesystem date_time)
	endif()
	list(APPEND Boost_NEEDED_COMPONENTS system chrono filesystem date_time)
    #list(REMOVE_DUPLICATES Boost_NEEDED_COMPONENTS)

	if (WIN32)
    	win3rdParty(Boost VCID TIMEOUT 600 #VERBOSE ON
    	    MSVC11 "win3rdParty/MSVC11/Boost" "https://gforge.inria.fr/frs/download.php/file/35598/boost_1_55_0.7z"
    	    MSVC12 "win3rdParty/MSVC11/Boost" "https://gforge.inria.fr/frs/download.php/file/35598/boost_1_55_0.7z"
    	    MSVC14 "win3rdParty/MSVC14/Boost" "https://gforge.inria.fr/frs/download.php/file/36899/boost-1.64.7z"    # boost compatible with msvc14
    	)
    	if(WIN32 AND NOT Boost_WIN3RDPARTY_VCID AND Boost_WIN3RDPARTY_USE)
    	    message(WARNING "Boost_COMPILER is not set and it's needed. Try to disable Boost_WIN3RDPARTY_USE and set it manually.")
    	endif()

        # boost multiset arguments
        if (MSVC11 OR MSVC12)
            set(boost_multiset_arguments 
                    CHECK_CACHED_VAR BOOST_ROOT                 PATH "boost_1_55_0"
                    CHECK_CACHED_VAR BOOST_INCLUDEDIR 		    PATH "boost_1_55_0"
                    CHECK_CACHED_VAR BOOST_LIBRARYDIR 		    PATH "boost_1_55_0/${LIB_BUILT_DIR}"
                    #CHECK_CACHED_VAR Boost_COMPILER             STRING "-${Boost_WIN3RDPARTY_VCID}" DOC "vcid (eg: -vc110 for MSVC11)"
                    CHECK_CACHED_VAR Boost_COMPILER             STRING "-vc110" DOC "vcid (eg: -vc110 for MSVC11)" # NOTE: if it doesnt work, uncomment this option and set the right value for VisualC id
                )
        elseif (MSVC14)
            set(boost_multiset_arguments 
                    CHECK_CACHED_VAR BOOST_ROOT                 PATH "boost-1.64"
                    CHECK_CACHED_VAR BOOST_INCLUDEDIR 		    PATH "boost-1.64"
                    CHECK_CACHED_VAR BOOST_LIBRARYDIR 		    PATH "boost-1.64/${LIB_BUILT_DIR}"
                    CHECK_CACHED_VAR Boost_COMPILER             STRING "-vc141" DOC "vcid (eg: -vc110 for MSVC11)" # NOTE: if it doesnt work, uncomment this option and set the right value for VisualC id
                )
        else ()
            message("There is no provided Boost library for your version of MSVC")
        endif()

        # list(APPEND boost_multiset_arguments
    	#         CHECK_CACHED_VAR Boost_NO_SYSTEM_PATHS      BOOL ON DOC "Set to ON to disable searching in locations not specified by these boost cached hint variables"
    	#         CHECK_CACHED_VAR Boost_NO_BOOST_CMAKE       BOOL ON DOC "Set to ON to disable the search for boost-cmake (package cmake config file if boost was built with cmake)"
    	#         CHECK_CACHED_VAR Boost_REQUIRED_COMPONENTS  STRING LIST "${Boost_NEEDED_COMPONENTS}"
        # )

    	win3rdParty(Boost MULTI_SET ${boost_multiset_arguments}
                # TODO SV: keep compatibility with old version
                # # CHECK_CACHED_VAR BOOST_ROOT                 PATH "boost_1_55_0"
    	        # # CHECK_CACHED_VAR BOOST_INCLUDEDIR 		    PATH "boost_1_55_0"
    	        # # CHECK_CACHED_VAR BOOST_LIBRARYDIR 		    PATH "boost_1_55_0/${LIB_BUILT_DIR}"
    	        # CHECK_CACHED_VAR BOOST_ROOT                 PATH "boost-1.64"
    	        # CHECK_CACHED_VAR BOOST_INCLUDEDIR 		    PATH "boost-1.64"
    	        # CHECK_CACHED_VAR BOOST_LIBRARYDIR 		    PATH "boost-1.64/${LIB_BUILT_DIR}"
    	        CHECK_CACHED_VAR Boost_NO_SYSTEM_PATHS      BOOL ON DOC "Set to ON to disable searching in locations not specified by these boost cached hint variables"
    	        CHECK_CACHED_VAR Boost_NO_BOOST_CMAKE       BOOL ON DOC "Set to ON to disable the search for boost-cmake (package cmake config file if boost was built with cmake)"
    	        # #CHECK_CACHED_VAR Boost_COMPILER             STRING "-${Boost_WIN3RDPARTY_VCID}" DOC "vcid (eg: -vc110 for MSVC11)"
                # # CHECK_CACHED_VAR Boost_COMPILER             STRING "-vc110" DOC "vcid (eg: -vc110 for MSVC11)" # NOTE: if it doesnt work, uncomment this option and set the right value for VisualC id
                # CHECK_CACHED_VAR Boost_COMPILER             STRING "-vc141" DOC "vcid (eg: -vc110 for MSVC11)" # NOTE: if it doesnt work, uncomment this option and set the right value for VisualC id
    	        CHECK_CACHED_VAR Boost_REQUIRED_COMPONENTS  STRING LIST "${Boost_NEEDED_COMPONENTS}"
    	)
    	if(NOT Boost_COMPILER AND Boost_WIN3RDPARTY_USE)
    	    message(WARNING "Boost_COMPILER is not set and it's needed.")
    	endif()
	else()
		set(Boost_REQUIRED_COMPONENTS ${Boost_NEEDED_COMPONENTS})
	endif()

    if(Boost_NEEDED_COMPONENTS)
        find_package(Boost REQUIRED COMPONENTS ${Boost_REQUIRED_COMPONENTS})
    else()
        find_package(Boost)
    endif()

	if(Boost_LIB_DIAGNOSTIC_DEFINITIONS)
        add_definitions(${Boost_LIB_DIAGNOSTIC_DEFINITIONS})
    endif()

    #if(WIN32)
        add_definitions(-DBOOST_ALL_DYN_LINK -DBOOST_ALL_NO_LIB)
    #endif()

	if(Boost_FOUND OR BOOST_FOUND)
        if(CGAL_FOUND)
            list(APPEND CGAL_LIBRARIES ${Boost_LIBRARIES})
        endif()
	else()
		message(SEND_ERROR "Boost not found. Set BOOST_ROOT or BOOST_DIR and Boost_LIBRARYDIR.")
		message("CGAL use boost and if it is not build with embedded static boost lib, you will need it at runtime.")
		set(BUILD_IBR_CROWD			OFF)
		message("BUILD_IBR_CROWD		= ${BUILD_IBR_CROWD}")
	endif()

	include_directories(${BOOST_INCLUDEDIR} ${Boost_INCLUDE_DIRS})
	link_directories(${BOOST_LIBRARYDIR} ${Boost_LIBRARY_DIRS})

endif()


##############
## Find OpenMP
##############
find_package(OpenMP)
## then into your sub-CMakeLists.txt you can use :
## SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES COMPILE_OPTIONS ${OpenMP_CXX_FLAGS} LINK_FLAGS ${OpenMP_CXX_FLAGS})
## OR
## #target_compile_options(${PROJECT_NAME} PUBLIC ${OpenMP_C_FLAGS} PUBLIC ${OpenMP_CXX_FLAGS}) # but on some plateform there is a risk to skip LINK_FLAGS...


###################
## Find OpenMesh
###################
sibr_addlibrary(
    NAME OpenMeshCore
    MSVC11 "https://gforge.inria.fr/frs/download.php/file/37429/OpenMesh.7z"
    MSVC14 "https://gforge.inria.fr/frs/download.php/file/37429/OpenMesh.7z"     # TODO SV: provide a valid version if required
    REQUIREDFOR BUILD_IBR_DEPTHMESHER
)

sibr_addlibrary(
    NAME NativeFileDialog
    MSVC14 "https://gforge.inria.fr/frs/download.php/file/37621/nfd.7z"  
    REQUIREDFOR BUILD_SIBR
)

##############
## Find OpenCV
##############
if(1)#BUILD_MVIIR OR BUILD_VBR OR BUILD_IBR_UTILITIES_MVSEG OR BUILD_UTILITIES_DEPTH_SYNTH OR BUILD_IBR_UTILITIES_UNDISTORT)

    if (MSVC11 OR MSVC12)
        set(opencv_set_arguments 
            CHECK_CACHED_VAR OpenCV_DIR PATH "opencv/build" ## see OpenCVConfig.cmake
        )
    elseif (MSVC14)
        set(opencv_set_arguments 
            CHECK_CACHED_VAR OpenCV_DIR PATH "opencv-3.4.1/build" ## see OpenCVConfig.cmake
        )
    else ()
        message("There is no provided OpenCV library for your version of MSVC")
    endif()

    win3rdParty(OpenCV #VERBOSE ON
            MSVC11 "win3rdParty/MSVC11/OpenCV" "https://gforge.inria.fr/frs/download.php/file/35599/opencv.7z"
            MSVC12 "win3rdParty/MSVC11/OpenCV" "https://gforge.inria.fr/frs/download.php/file/35599/opencv.7z"
            MSVC14 "win3rdParty/MSVC14/OpenCV" "https://gforge.inria.fr/frs/download.php/file/37552/opencv-3.4.1.7z"    # opecv compatible with msvc14 and with contribs
            SET ${opencv_set_arguments}
        )
    find_package(OpenCV REQUIRED) ## Use directly the OpenCVConfig.cmake provided
    
     ##https://stackoverflow.com/questions/24262081/cmake-relwithdebinfo-links-to-debug-libs
    set_target_properties(${OpenCV_LIBS} PROPERTIES MAP_IMPORTED_CONFIG_RELWITHDEBINFO RELEASE)    
   
    if(OpenCV_INCLUDE_DIRS)
        foreach(inc ${OpenCV_INCLUDE_DIRS})
            if(NOT EXISTS ${inc})
                set(OpenCV_INCLUDE_DIR "" CACHE PATH "additional custom include DIR (in case of trouble to find it (fedora 17 opencv package))")
            endif()
        endforeach()
        if(OpenCV_INCLUDE_DIR)
            list(APPEND OpenCV_INCLUDE_DIRS ${OpenCV_INCLUDE_DIR})
            include_directories(${OpenCV_INCLUDE_DIRS})
        endif()
    endif()
endif()

###################
## Find GLFW
###################
sibr_addlibrary(
    NAME GLFW
    MSVC11 "https://gforge.inria.fr/frs/download.php/file/36912/glfw-3.2.1.7z"
    MSVC14 "https://gforge.inria.fr/frs/download.php/file/36912/glfw-3.2.1.7z"     # TODO SV: provide a valid version if required
    REQUIREDFOR BUILD_SIBR
)


if (BUILD_IBR_TFGL_INTEROP)
    sibr_addlibrary(
        NAME tfgl_interop
        MSVC11 "https://gforge.inria.fr/frs/download.php/file/37881/tfinterop.7z"
        MSVC14 "https://gforge.inria.fr/frs/download.php/file/37881/tfinterop.7z"  
        REQUIREDFOR BUILD_IBR_TFGL_INTEROP
    )
endif()




if(BUILD_IS)
	find_package(VRPN REQUIRED)
	if(QUAT_INCLUDE_DIR AND VRPN_INCLUDE_DIR)
		include_directories(${QUAT_INCLUDE_DIR})
		include_directories(${VRPN_INCLUDE_DIR})
	endif()
	mark_as_advanced(VRPN_DIR)
	mark_as_advanced(VRPN_LIBRARY)
	mark_as_advanced(QUAT_LIBRARY)
	mark_as_advanced(VRPN_INCLUDE_DIR)
	mark_as_advanced(QUAT_INCLUDE_DIR)
endif()


if(BUILD_IBR_CROWD)

    #################
    ## Find FreeImage
    #################
    win3rdParty(FreeImage #VERBOSE ON
        MSVC11 "win3rdParty/MSVC11/FreeImage" "https://gforge.inria.fr/frs/download.php/file/34367/FreeImage.7z"
        #MSVC12 "win3rdParty/MSVC12/FreeImage" "TODO"
        MSVC14 "win3rdParty/MSVC14/FreeImage" "https://gforge.inria.fr/frs/download.php/file/34367/FreeImage.7z"    # TODO SV: provide a valid version if required
        MULTI_SET CHECK_CACHED_VAR FreeImage_DIR PATH "FreeImage"
    )
	find_package(FreeImage REQUIRED)
	if(FreeImage_FOUND)
		include_directories(${FreeImage_INCLUDE_DIR})
	else()
		message(SEND_ERROR "FreeImage not found.")
		set(BUILD_IBR_CROWD			OFF)
		message("BUILD_IBR_CROWD		= ${BUILD_IBR_CROWD}")
	endif()


    ###############
    ## Find TinyXml
    ###############
    win3rdParty(TinyXml #VERBOSE ON
        MSVC11 "win3rdParty/MSVC11/TinyXml" "https://gforge.inria.fr/frs/download.php/file/34366/TinyXML.7z"
        MSVC12 "win3rdParty/MSVC11/TinyXml" "https://gforge.inria.fr/frs/download.php/file/34366/TinyXML.7z"
        MSVC14 "win3rdParty/MSVC14/TinyXml" "https://gforge.inria.fr/frs/download.php/file/34366/TinyXML.7z"    # TODO SV: provide a valid version if required
        MULTI_SET CHECK_CACHED_VAR TinyXml_DIR PATH "TinyXML"
    )
	find_package(TinyXml REQUIRED COMPONENTS STL)
	if(TinyXml_FOUND)
		include_directories(${TinyXml_INCLUDE_DIR})
	else()
		message(SEND_ERROR "TinyXml not found.")
		set(BUILD_IBR_CROWD			OFF)
		message("BUILD_IBR_CROWD		= ${BUILD_IBR_CROWD}")
	endif()


#    ###########
#    ## Find GLM
#    ###########
#    win3rdParty(GLM #VERBOSE ON
#        MSVC11 "win3rdParty/MSVC11/GLM" ""
#        #MSVC12 "win3rdParty/MSVC12/GLM" "TODO"
#        MULTI_SET CHECK_CACHED_VAR GLM_DIR PATH "glm"
#    )
#	find_package(GLM REQUIRED)
#	if(GLM_FOUND)
#		include_directories(${GLM_INCLUDE_DIR})
#	else()
#		message(SEND_ERROR "GLM not found.")
#		set(BUILD_IBR_CROWD			OFF)
#		message("BUILD_IBR_CROWD		= ${BUILD_IBR_CROWD}")
#	endif()

    ##################
    ## Find WildMagic4
    ##################
    win3rdParty(WildMagic4 #VERBOSE ON
        MSVC11 "win3rdParty/MSVC11/WildMagic4" "https://gforge.inria.fr/frs/download.php/file/34364/WildMagic4.7z"
		MSVC12 "win3rdParty/MSVC11/WildMagic4" "https://gforge.inria.fr/frs/download.php/file/34364/WildMagic4.7z"
		MSVC14 "win3rdParty/MSVC14/WildMagic4" "https://gforge.inria.fr/frs/download.php/file/34364/WildMagic4.7z"    # TODO SV: provide a valid version if required
        MULTI_SET CHECK_CACHED_VAR WildMagic4_DIR PATH "WildMagic4"
    )
	find_package(WildMagic4 REQUIRED)
	if(WildMagic4_FOUND)
		include_directories(${WildMagic4_INCLUDE_DIR})
	else()
		message(SEND_ERROR "WildMagic4 not found.")
		set(BUILD_IBR_CROWD			OFF)
		message("BUILD_IBR_CROWD		= ${BUILD_IBR_CROWD}")
	endif()


    #################
    ## Find Behaviour
    #################
    win3rdParty(Behaviour #VERBOSE ON
        MSVC11 "win3rdParty/MSVC11/Behaviour" "https://gforge.inria.fr/frs/download.php/file/34363/Behaviour.7z"
		MSVC12 "win3rdParty/MSVC11/Behaviour" "https://gforge.inria.fr/frs/download.php/file/34363/Behaviour.7z"
		MSVC14 "win3rdParty/MSVC14/Behaviour" "https://gforge.inria.fr/frs/download.php/file/34363/Behaviour.7z"    # TODO SV: provide a valid version if required
        MULTI_SET CHECK_CACHED_VAR Behaviour_DIR PATH "Behaviour"
    )
	find_package(Behaviour REQUIRED)
	if(Behaviour_FOUND)
		include_directories(${Behaviour_INCLUDE})
	else()
		message(SEND_ERROR "Behaviour not found.")
		set(BUILD_IBR_CROWD			OFF)
		message("BUILD_IBR_CROWD		= ${BUILD_IBR_CROWD}")
	endif()


    #####################
    ## Find NaturalMotion
    #####################
    win3rdParty(NaturalMotion #VERBOSE ON
        MSVC11 "win3rdParty/MSVC11/NaturalMotion" "https://gforge.inria.fr/frs/download.php/file/34365/NaturalMotion.7z"
		MSVC12 "win3rdParty/MSVC11/NaturalMotion" "https://gforge.inria.fr/frs/download.php/file/34365/NaturalMotion.7z"
		MSVC14 "win3rdParty/MSVC14/NaturalMotion" "https://gforge.inria.fr/frs/download.php/file/34365/NaturalMotion.7z"    # TODO SV: provide a valid version if required
        MULTI_SET CHECK_CACHED_VAR NaturalMotion_DIR PATH "NaturalMotion"
    )
	find_package(NaturalMotion REQUIRED)
	if(NaturalMotion_FOUND)
		include_directories(${NaturalMotion_INCLUDE_DIRS})
	else()
		message(SEND_ERROR "NaturalMotion not found.")
		set(BUILD_IBR_CROWD			OFF)
		message("BUILD_IBR_CROWD		= ${BUILD_IBR_CROWD}")
	endif()

endif()

if (BUILD_REFLECTION)
    #####################
    ## Find TBB
    #####################
    win3rdParty(TBB #VERBOSE ON
		MSVC14 "win3rdParty/MSVC14/TBB" "https://gforge.inria.fr/frs/download.php/file/37096/tbb2017_U7.7z"
        MULTI_SET CHECK_CACHED_VAR TBB_DIR PATH "TBB"
    )
    find_package(TBB REQUIRED)
    include_directories(${TBB_INCLUDE_DIR})

    #####################
    ## Find MapmapCPU
    ## https://github.com/dthuerck/mapmap_cpu
    ## Using Theo's version
    #####################
    win3rdParty(MapmapCPU #VERBOSE ON
        MSVC14 "win3rdParty/MSVC14/MapmapCPU" "https://gforge.inria.fr/frs/download.php/file/37030/mapmap_2017_08_29_TTv2.zip"
        MULTI_SET CHECK_CACHED_VAR MapmapCPU_DIR PATH "MapmapCPU"
    )
    find_package(MapmapCPU REQUIRED)
    include_directories(${MapmapCPU_INCLUDE_DIR})


endif ()

Win3rdPartyGlobalCacheAction()
message(STATUS "[dependencies] Finish to look for necessary dependencies.")
message("")
