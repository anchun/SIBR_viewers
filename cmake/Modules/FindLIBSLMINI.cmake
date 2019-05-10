## libsl-mini
##
## This file use the LIBSLMINI_DIR cmake variable as input to search files/dirs
##
## This file may provides:
##  * LIBSLMINI_GL_HEADERS_ONLY_DIR : this is the include dir for OpenGL compatibility
##  * LIBSLMINI_QT_HEADERS_ONLY_DIR : this is the include dir for Qt/GL compatibility
##
## example 1 :
## find_package(LIBSLMINI REQUIRED QT)
## if(LIBSLMINI_FOUND)
## 	INCLUDE_DIRECTORIES(${LIBSLMINI_QT_HEADERS_ONLY_DIR})
## endif()
##
## example 2 :
## find_package(LIBSLMINI REQUIRED GL)
## if(LIBSLMINI_FOUND)
## 	INCLUDE_DIRECTORIES(${LIBSLMINI_GL_HEADERS_ONLY_DIR})
## endif()
##
## example 3 :
## find_package(LIBSLMINI)
## if(LIBSLMINI_FOUND)
## 	INCLUDE_DIRECTORIES(${LIBSLMINI_GL_HEADERS_ONLY_DIR})
## 	INCLUDE_DIRECTORIES(${LIBSLMINI_QT_HEADERS_ONLY_DIR})
## endif()
##
## This file declare a LIBSLMINI_DOWNLOAD_OVERWRITE cache GUI cmake flag allowing
##	to download the libsl-mini public svn project in the BINARY_DIR and set the LIBSLMINI_DIR.
##	So if you want to update this project, be sur you have no modification no commited/pushed
##	and use this (one shot) flag to re-download and overwrite the libsl-mini project.
##
## NOTE:
## libsl-mini LIBSLMINI_GL_HEADERS_ONLY_DIR use CImg.h internaly 
## which need to use external tools like ImageMagick or GraphicMagick.
##
## Written by jerome.esnault@inria.fr

## set SVN_URL and SVN_PATH according to LIBSLMINI_DL_* variable
SET(SVN_URL 	svn://scm.gforge.inria.fr/svnroot/libsl-mini/trunk)
SET(SVN_PATH 	"${CMAKE_BINARY_DIR}/libsl-mini")

set(PROGRAMFILESx86 "PROGRAMFILES(x86)")

##########################################
MACRO(libslmini_download SVN_URL SVN_PATH SVN_UPDATE_BOOL)
	MESSAGE(STATUS "LIBSLMINI_DOWNLOAD_OVERWRITE=${LIBSLMINI_DOWNLOAD_OVERWRITE}. CMake will download/update for you the libsl-mini project")

	## clear if needed
	IF(EXISTS "${SVN_PATH}" AND LIBSLMINI_DOWNLOAD_OVERWRITE)
		execute_process(COMMAND ${CMAKE_COMMAND} -E remove_directory "${SVN_PATH}")
	ENDIF()
	IF(DEFINED LIBSLMINI_GL_HEADERS_ONLY_DIR)
        UNSET(LIBSLMINI_GL_HEADERS_ONLY_DIR CACHE)
	ENDIF()
	IF(DEFINED LIBSLMINI_QT_HEADERS_ONLY_DIR)
        UNSET(LIBSLMINI_QT_HEADERS_ONLY_DIR CACHE)
	ENDIF()

	## find SVN_CMD (classic svn or TortoiseSVN)
    FIND_PACKAGE(Subversion)
	IF(NOT Subversion_SVN_EXECUTABLE)
		IF(WIN32)
            FIND_PROGRAM(Tortoise_SVN_EXECUTABLE NAMES TortoiseProc PATHS "$ENV{PROGRAMFILES}/TortoiseSVN/bin" "$ENV{${PROGRAMFILESx86}}/TortoiseSVN/bin" "$ENV{ProgramW6432}/TortoiseSVN/bin")
			IF(Tortoise_SVN_EXECUTABLE)
				mark_as_advanced(Subversion_SVN_EXECUTABLE)
				mark_as_advanced(Tortoise_SVN_EXECUTABLE)
				file(TO_NATIVE_PATH ${Tortoise_SVN_EXECUTABLE} Tortoise_SVN_EXECUTABLE)
				## see : http://tortoisesvn.net/docs/release/TortoiseSVN_en/tsvn-automation.html
                SET(SVN_CMD "${Tortoise_SVN_EXECUTABLE}" /command:checkout /path:${SVN_PATH} /url:${SVN_URL} /closeforlocal /closeonend:1)
				SET(SVN_UPDATE_CMD "${Tortoise_SVN_EXECUTABLE}" /command:update /path:${SVN_PATH} /closeforlocal /closeonend:3)
				SET(SVN_CMD_TIMEOUT 120)
				MESSAGE("A USER INTERACTION MAY BE NEEDED to accept the tortoise (GUI) SVN checkout command (process will endup in ${SVN_CMD_TIMEOUT}s...)\n=> Click OK please...")
			else()
				SET(complementWinMsg "\nsvn download link for windows: https://gforge.inria.fr/scm/?group_id=5238.\nYou can also try to get TortoiseSVN (http://tortoisesvn.net/downloads.html).")
			ENDIF()
		ENDIF()
	else()
		mark_as_advanced(Subversion_SVN_EXECUTABLE)
		file(TO_NATIVE_PATH ${Subversion_SVN_EXECUTABLE} Subversion_SVN_EXECUTABLE)
        SET(SVN_CMD "${Subversion_SVN_EXECUTABLE}" checkout --username anonsvn --password anonsvn ${SVN_URL} ${SVN_PATH} --no-auth-cache)
		SET(SVN_UPDATE_CMD "${Subversion_SVN_EXECUTABLE}" update ${SVN_PATH})
        SET(SVN_CMD_TIMEOUT 120)
    ENDIF()

	if(NOT SVN_CMD)
		MESSAGE(WARNING "error: could not find svn for checkout libsl-mini.\nSET Subversion_SVN_EXECUTABLE or try to download svn.\n${complementWinMsg}")
	endif()
	
	## download libsl-mini to CMAKE_BINARY_DIR
	if(SVN_CMD AND NOT ${SVN_UPDATE_BOOL})
		message(STATUS "${SVN_CMD}")
		execute_process(COMMAND ${SVN_CMD} WORKING_DIRECTORY "${CMAKE_BINARY_DIR}" TIMEOUT ${SVN_CMD_TIMEOUT} RESULT_VARIABLE resVar OUTPUT_VARIABLE outVar ERROR_VARIABLE errVar)
		IF(${resVar} MATCHES "0")
			MESSAGE(STATUS "${outVar}\nDownload succeed")
			SET(LIBSLMINI_DOWNLOAD_OVERWRITE OFF CACHE BOOL "auto download libsl-mini (overwrite IF exist)" FORCE)
		else()
			MESSAGE(WARNING "DOWNLOAD failed or canceled:\nresVar=${resVar}\noutVar=${outVar}\nerrVar=${errVar}")
            MESSAGE("You may need to \"svn checkout ${SVN_URL} ${SVN_PATH}\" by yourself.")
		ENDIF()
	elseif(SVN_CMD AND ${SVN_UPDATE_BOOL})
		message(STATUS "${SVN_UPDATE_CMD}")
		execute_process(COMMAND ${SVN_UPDATE_CMD} WORKING_DIRECTORY "${CMAKE_BINARY_DIR}" TIMEOUT ${SVN_CMD_TIMEOUT} RESULT_VARIABLE resVar OUTPUT_VARIABLE outVar ERROR_VARIABLE errVar)
		IF(${resVar} MATCHES "0")
			MESSAGE(STATUS "${outVar}\nUpdate succeed")
		else()
			MESSAGE(WARNING "UPDATE failed or canceled:\nresVar=${resVar}\noutVar=${outVar}\nerrVar=${errVar}")
            MESSAGE("You may need to \"svn update ${SVN_PATH}\" by yourself.")
		ENDIF()		
	ENDIF()

ENDMACRO()
##########################################


## the check and/or download part (default no download to let priority to user to SET LIBSLMINI_DIR by himself)
SET(LIBSLMINI_DOWNLOAD_OVERWRITE OFF CACHE BOOL "auto download libsl-mini (overwrite IF exist)")
IF(LIBSLMINI_DOWNLOAD_OVERWRITE)
	if(SVN_URL AND SVN_PATH)
		libslmini_download(${SVN_URL} ${SVN_PATH} OFF) ## just DL (not update)
	else()
		message(WARNING "something wrong: can't download without URL and PATH destination")
	endif()
ENDIF()


##########################################
## The real find part
MACRO(libslmini_find)
	file(GLOB_RECURSE LIBSLMINI_CONTENT "${SVN_PATH}/*.h")
	IF(LIBSLMINI_CONTENT AND NOT LIBSLMINI_DIR)
		SET(LIBSLMINI_DIR "${SVN_PATH}" CACHE PATH "path to the root libsl-mini project" FORCE)
	ENDIF()
		
	IF(NOT LIBSLMINI_GL_HEADERS_ONLY_DIR)
		FIND_PATH(LIBSLMINI_GL_HEADERS_ONLY_DIR
			NAMES			libsl.h
			PATHS			/usr
							/usr/opengl-header-only
							/usr/local
							/usr/local/opengl-header-only
							/opt/local
							/opt/local/opengl-header-only
							"${LIBSLMINI_DIR}"
							"${LIBSLMINI_DIR}/opengl-header-only"
							"$ENV{LIBSLMINI_DIR}"
							"$ENV{LIBSLMINI_DIR}/opengl-header-only"
			PATH_SUFFIXES 	include
		)
	ENDIF()
	
	IF(NOT LIBSLMINI_QT_HEADERS_ONLY_DIR)
		FIND_PATH(LIBSLMINI_QT_HEADERS_ONLY_DIR
			NAMES			opengl_functions.h
			PATHS			/usr
							/usr/qtopengl-header-only
							/usr/local
							/usr/local/qtopengl-header-only
							/opt/local
							/opt/local/qtopengl-header-only
							"${LIBSLMINI_DIR}"
							"${LIBSLMINI_DIR}/qtopengl-header-only"
							"$ENV{LIBSLMINI_DIR}"
							"$ENV{LIBSLMINI_DIR}/qtopengl-header-only"
			PATH_SUFFIXES 	include
		)
	ENDIF()
ENDMACRO()
##########################################

libslmini_find()

## IF not found, auto try to download
SET(LIBSLMINI_AUTO_UPDATE ON CACHE BOOL "auto update libsl-mini to the last version")
IF(NOT LIBSLMINI_DIR OR NOT EXISTS ${LIBSLMINI_DIR})
	if(SVN_URL AND SVN_PATH)
		libslmini_download(${SVN_URL} ${SVN_PATH} OFF) ## just DL (not update)
	else()
		message(WARNING "something wrong: can't download without URL and PATH destination")
	endif()
elseif(EXISTS "${SVN_PATH}/.svn" AND LIBSLMINI_AUTO_UPDATE)
	if(SVN_URL AND SVN_PATH)
		libslmini_download(${SVN_URL} ${SVN_PATH} ON) ## not DL but just update
	else()
		message(WARNING "something wrong: can't download without URL and PATH destination")
	endif()
ENDIF()

libslmini_find()


if(LIBSLMINI_FIND_REQUIRED)
	if(LIBSLMINI_FIND_REQUIRED_GL)
		list(APPEND LIBSLMINI_REQUIRED_LIST LIBSLMINI_GL_HEADERS_ONLY_DIR)
	endif()
	if(LIBSLMINI_FIND_REQUIRED_QT)
		list(APPEND LIBSLMINI_REQUIRED_LIST LIBSLMINI_QT_HEADERS_ONLY_DIR)
	endif()
	if(NOT LIBSLMINI_FIND_REQUIRED_QT AND NOT LIBSLMINI_FIND_REQUIRED_GL)
		list(APPEND LIBSLMINI_REQUIRED_LIST LIBSLMINI_QT_HEADERS_ONLY_DIR LIBSLMINI_GL_HEADERS_ONLY_DIR)	
	endif()
else()
	list(APPEND LIBSLMINI_REQUIRED_LIST LIBSLMINI_QT_HEADERS_ONLY_DIR LIBSLMINI_GL_HEADERS_ONLY_DIR)
endif()


## Additional check due to the CImg from libsl-mini GL headers which need external image tool
list(FIND LIBSLMINI_REQUIRED_LIST LIBSLMINI_GL_HEADERS_ONLY_DIR USE_CIMG)
if(${USE_CIMG} MATCHES "-1")
	## CImg.h isn't used
else() 
	## CImg.h is used, so ImageMagick or GraphicsMagick is needed
	find_package(ImageMagick QUIET COMPONENTS convert identify mogrify)
	find_package(GraphicsMagick QUIET)
	IF(NOT ImageMagick_FOUND AND NOT GraphicsMagick_FOUND)
        message(WARNING "
        You need at least :
        GraphicsMagick (http://www.graphicsmagick.org/download.html)
        or
        ImageMagick (http://www.imagemagick.org/script/binary-releases.php)
            to be able to run the application (because of CImg from libsl-mini GL headers)
            ")
		SET(GraphicsMagick_DIR "$ENV{GraphicsMagick_DIR}" CACHE PATH "The GraphicsMagick Current BinPath")
        SET(ImageMagick_DIR "$ENV{ImageMagick_DIR}" CACHE PATH "The ImageMagick Current BinPath")
    ELSE()
            mark_as_advanced(GraphicsMagick_DIR)
            mark_as_advanced(ImageMagick_DIR)
    ENDIF()
endif()


INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(LIBSLMINI
	REQUIRED_VARS ${LIBSLMINI_REQUIRED_LIST}
    FAIL_MESSAGE "Mini LibSL wasn't found correctly. Set LIBSLMINI_DIR to the root dir or LIBSLMINI_DOWNLOAD_OVERWRITE to ON"
)
