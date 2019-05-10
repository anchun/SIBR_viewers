## Based on the FindSuiteSparse.cmake written by jerome.esnault@inria.fr
## Written by clement.riant@inria.fr

if(NOT NaturalMotion_DIR)
    set(NaturalMotion_DIR "$ENV{NaturalMotion_DIR}" CACHE PATH "NaturalMotion root directory")
endif()
if(NaturalMotion_DIR)
	file(TO_CMAKE_PATH ${NaturalMotion_DIR} NaturalMotion_DIR)
endif()

## set default verbosity
if(NOT NaturalMotion_VERBOSE)
	set(NaturalMotion_VERBOSE OFF)
else()
	message(STATUS "Start to FindNaturalMotion.cmake :")
endif()


## set the LIB POSTFIX to find in a right directory according to what kind of compiler we use (32/64bits)
if(CMAKE_SIZEOF_VOID_P EQUAL 8)  # Size in bytes!
	set(NaturalMotion_SEARCH_LIB_POSTFIX "64" CACHE STRING "suffix for 32/64 dir placement")
else()  # Size in bytes!
	set(NaturalMotion_SEARCH_LIB_POSTFIX "86" CACHE STRING "suffix for 32/64 dir placement")
endif()
if(NaturalMotion_SEARCH_LIB_POSTFIX)
	mark_as_advanced(NaturalMotion_SEARCH_LIB_POSTFIX)
	if(NaturalMotion_VERBOSE)
		message(STATUS "   find_library will search inside lib${NaturalMotion_SEARCH_LIB_POSTFIX} directory (can be changed with NaturalMotion_SEARCH_LIB_POSTFIX)")
	endif()
endif()

## This utility macro is used to find all behavior projects by giving its name
## Since the name structure is the same for lib name and include dir name,
## we can use a generic way to find all of these with simple cmake lines of code
macro(NaturalMotion_FIND_COMPONENTS )

		
	## Look for each component the same way :
	##  * For include dir the reference file is the <component>.h
	##	* for library fileName the reference is the <component> itself (cmake will prepend/append necessary prefix/suffix according to the plateform)
	foreach(naturalMotionComp ${NaturalMotion_FIND_COMPONENTS})

		## used to construct specific cmake variables (in upper case) according to the component, but also used for find_*()
		string(TOUPPER ${naturalMotionComp} naturalMotionCompUC)
		string(TOLOWER ${naturalMotionComp} naturalMotionCompLC)

		## try to find include dir (looking for very important header file)
		find_path(NaturalMotion_${naturalMotionCompUC}_INCLUDE_DIR	
			NAMES 		${naturalMotionComp}/${naturalMotionComp}Utils.h
						morpheme/mrUtils.h
						comms/morphemeCommsTarget.h
						${naturalMotionComp}/${naturalMotionComp}.h
						NMGeomUtils/NMGeomUtils.h
			PATHS		${NaturalMotion_DIR}/common/include
						${NaturalMotion_DIR}/common/${naturalMotionComp}/include
						${NaturalMotion_DIR}/morpheme/SDK/${naturalMotionComp}/include
						${NaturalMotion_DIR}/common/${naturalMotionCompUC}/include
						${NaturalMotion_DIR}/morpheme/SDK/${naturalMotionCompUC}/include
						${NaturalMotion_DIR}/common/${naturalMotionCompLC}/include
						${NaturalMotion_DIR}/morpheme/SDK/${naturalMotionCompLC}/include
						${NaturalMotion_DIR}/morpheme/utils/${naturalMotionComp}/include
						${NaturalMotion_DIR}/morpheme/utils/${naturalMotionCompLC}/include
						${NaturalMotion_DIR}/morpheme/utils/${naturalMotionCompUC}/include
		)
		## check if found
		if(NOT NaturalMotion_${naturalMotionCompUC}_INCLUDE_DIR)
			message(WARNING "   Failed to find ${naturalMotionComp} :\nNaturalMotion_${naturalMotionCompUC}_INCLUDE_DIR not found.\nCheck you write correctly the component name (case sensitive),\nor set the NaturalMotion_${naturalMotionCompUC}_DIR to look inside")
		else()
			list(APPEND NaturalMotion_INCLUDE_DIRS	${NaturalMotion_${naturalMotionCompUC}_INCLUDE_DIR})
		endif()

		## try to find filepath lib name (looking for very important lib file)
		find_library(NaturalMotion_${naturalMotionCompUC}_LIBRARY_RELEASE 
			NAMES 			morpheme${naturalMotionComp}_NoPhysics_vc110_x${NaturalMotion_SEARCH_LIB_POSTFIX} ${naturalMotionComp}_NoPhysics_vc110_x${NaturalMotion_SEARCH_LIB_POSTFIX}  
			PATHS 			${NaturalMotion_DIR}/lib
		)
		#message(WARNING "Search ${naturalMotionComp}_vc110_x${NaturalMotion_SEARCH_LIB_POSTFIX}_d")
		find_library(NaturalMotion_${naturalMotionCompUC}_LIBRARY_DEBUG 
			NAMES 			morpheme${naturalMotionComp}_NoPhysics_vc110_x${NaturalMotion_SEARCH_LIB_POSTFIX}_d ${naturalMotionComp}_NoPhysics_vc110_x${NaturalMotion_SEARCH_LIB_POSTFIX}_d
			PATHS 			${NaturalMotion_DIR}/lib
		)
		
		## check and auto complete release with debug if release missing and vice versa
		
		if(NaturalMotion_${naturalMotionCompUC}_LIBRARY_RELEASE)
			if(NOT NaturalMotion_${naturalMotionCompUC}_LIBRARY_DEBUG)
				set(NaturalMotion_${naturalMotionCompUC}_LIBRARY_DEBUG ${NaturalMotion_${naturalMotionCompUC}_LIBRARY_RELEASE} CACHE PATH "Path to a library." FORCE)
			endif()
		endif()
		if(NaturalMotion_${naturalMotionCompUC}_LIBRARY_DEBUG)
			if(NOT NaturalMotion_${naturalMotionCompUC}_LIBRARY_RELEASE)
				set(NaturalMotion_${naturalMotionCompUC}_LIBRARY_RELEASE ${NaturalMotion_${naturalMotionCompUC}_LIBRARY_DEBUG} CACHE PATH "Path to a library." FORCE)
			endif()
		endif()
		
		## check and append the and NaturalMotion_LIBRARIES list, and warn if not found (release and debug) otherwise
		if(NOT NaturalMotion_${naturalMotionCompUC}_LIBRARY_RELEASE AND NOT NaturalMotion_${naturalMotionCompUC}_LIBRARY_DEBUG)
			message(WARNING "   Failed to find ${naturalMotionComp} :
			Check you write correctly the component name (case sensitive),
			or set the NaturalMotion_${naturalMotionCompUC}_DIR to look inside,
			or set directly NaturalMotion_${naturalMotionCompUC}_LIBRARY_DEBUG and NaturalMotion_${naturalMotionCompUC}_LIBRARY_RELEASE
			")
		else()
			list(APPEND NaturalMotion_LIBRARIES	optimized "${NaturalMotion_${naturalMotionCompUC}_LIBRARY_RELEASE}" debug "${NaturalMotion_${naturalMotionCompUC}_LIBRARY_DEBUG}")
		endif()
		
		## here we allow to find at least the include OR the lib dir and just warn if one of both missing
		if(NOT NaturalMotion_${naturalMotionCompUC}_INCLUDE_DIR AND NOT NaturalMotion_${naturalMotionCompUC}_LIBRARY_RELEASE)
			set(NaturalMotion_${naturalMotionCompUC}_FOUND OFF)
		else()
			set(NaturalMotion_${naturalMotionCompUC}_FOUND ON)
		endif()
		
		## if one of both (include dir or filepath lib), then we provide a new cmake cache variable for the search. Otherwise we don't need anymore to expose all intermediates variables
		if(NOT NaturalMotion_${naturalMotionCompUC}_FOUND)
			set(NaturalMotion_${naturalMotionCompUC}_DIR "$ENV{NaturalMotion_${naturalMotionCompUC}_DIR}" CACHE PATH "${naturalMotionComp} root directory")
		else()
			mark_as_advanced(NaturalMotion_${naturalMotionCompUC}_INCLUDE_DIR)
			mark_as_advanced(NaturalMotion_${naturalMotionCompUC}_LIBRARY_RELEASE)
			mark_as_advanced(NaturalMotion_${naturalMotionCompUC}_LIBRARY_DEBUG)
			if(DEFINED NaturalMotion_${naturalMotionCompUC}_DIR)
				mark_as_advanced(NaturalMotion_${naturalMotionCompUC}_DIR)
			endif()
		endif()

		if(NaturalMotion_VERBOSE)
			message(STATUS "   NaturalMotion_${naturalMotionCompUC}_FOUND = ${NaturalMotion_${naturalMotionCompUC}_FOUND} : ")
			message(STATUS "      * NaturalMotion_${naturalMotionCompUC}_INCLUDE_DIR = ${NaturalMotion_${naturalMotionCompUC}_INCLUDE_DIR}")
			message(STATUS "      * NaturalMotion_${naturalMotionCompUC}_LIBRARY_DEBUG = ${NaturalMotion_${naturalMotionCompUC}_LIBRARY_DEBUG}")
			message(STATUS "      * NaturalMotion_${naturalMotionCompUC}_LIBRARY_RELEASE = ${NaturalMotion_${naturalMotionCompUC}_LIBRARY_RELEASE}")
		endif()
		
		list(APPEND NaturalMotion_FOUND_LIST NaturalMotion_${naturalMotionCompUC}_FOUND)
		
	endforeach()
	
	
	## set the final NaturalMotion_FOUND based on all previous components found (status)
	foreach(componentToCheck ${NaturalMotion_FOUND_LIST})
		set(NaturalMotion_FOUND ON)
		if(NOT ${componentToCheck})
			set(NaturalMotion_FOUND OFF)
			break() ## one component not found is enought to failed
		endif()
	endforeach()
endmacro()

## Default naturalMotion if user don't use the COMPONENTS flag in find_package(NaturalMotion ...) command
if(NOT NaturalMotion_FIND_COMPONENTS)
	#message(FATAL_ERROR "ok")
	list(APPEND NaturalMotion_FIND_COMPONENTS Comms2 Core NMRuntimeUtils SimpleBundle NMPlatform)
endif()

NaturalMotion_FIND_COMPONENTS()

if(NaturalMotion_INCLUDE_DIRS)
	list(REMOVE_DUPLICATES NaturalMotion_INCLUDE_DIRS)
	list(APPEND NaturalMotion_INCLUDE_DIRS "${NaturalMotion_DIR}/morpheme/SDK/")
	#message(FATAL_ERROR "ok ${NaturalMotion_INCLUDE_DIRS}")
endif()
if(NaturalMotion_LIBRARIES)
	list(REMOVE_DUPLICATES NaturalMotion_LIBRARIES)
endif()

if(NaturalMotion_VERBOSE)
	message(STATUS "Finish to FindNaturalMotion.cmake => NaturalMotion_FOUND=${NaturalMotion_FOUND}")
endif()

