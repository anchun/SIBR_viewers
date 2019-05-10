## Based on the FindSuiteSparse.cmake written by jerome.esnault@inria.fr
## Written by clement.riant@inria.fr

## check if global root SuiteSparse folder is set or not and cache it in order to let user fill it
if(NOT Behaviour_DIR)
    set(Behaviour_DIR "$ENV{Behaviour_DIR}" CACHE PATH "Behaviour root directory")
endif()
if(Behaviour_DIR)
	file(TO_CMAKE_PATH ${Behaviour_DIR} Behaviour_DIR)
endif()

## set default verbosity
if(NOT Behaviour_VERBOSE)
	set(Behaviour_VERBOSE OFF)
else()
	message(STATUS "Start to FindBehaviour.cmake :")
endif()


## set the LIB POSTFIX to find in a right directory according to what kind of compiler we use (32/64bits)
if(CMAKE_SIZEOF_VOID_P EQUAL 8)  # Size in bytes!
	set(Behaviour_SEARCH_LIB_POSTFIX "64" CACHE STRING "suffix for 32/64 dir placement")
else()  # Size in bytes!
	set(Behaviour_SEARCH_LIB_POSTFIX "86" CACHE STRING "suffix for 32/64 dir placement")
endif()
if(Behaviour_SEARCH_LIB_POSTFIX)
	mark_as_advanced(Behaviour_SEARCH_LIB_POSTFIX)
	if(Behaviour_VERBOSE)
		message(STATUS "   find_library will search inside lib${Behaviour_SEARCH_LIB_POSTFIX} directory (can be changed with Behaviour_SEARCH_LIB_POSTFIX)")
	endif()
endif()


## This utility macro is used to find all behavior projects by giving its name
## Since the name structure is the same for lib name and include dir name,
## we can use a generic way to find all of these with simple cmake lines of code
macro(Behaviour_FIND_COMPONENTS )

		
	## Look for each component the same way :
	##  * For include dir the reference file is the <component>.h
	##	* for library fileName the reference is the <component> itself (cmake will prepend/append necessary prefix/suffix according to the plateform)
	foreach(behaviorComp ${Behaviour_FIND_COMPONENTS})

		## used to construct specific cmake variables (in upper case) according to the component, but also used for find_*()
		string(TOUPPER ${behaviorComp} behaviorCompUC)
		string(TOLOWER ${behaviorComp} behaviorCompLC)

		## try to find include dir (looking for very important header file)
		find_path(Behaviour_${behaviorCompUC}_INCLUDE_DIR	
			NAMES 			${behaviorComp}_std.h ${behaviorCompLC}_std.h ${behaviorCompUC}_std.h
			PATHS			/opt/local/include
						/usr/include
						/usr/local/include
						/usr/include/behavior
						/usr/local/include/behavior
						/usr/include/${behaviorComp}
						/usr/local/include/${behaviorComp}
						${Behaviour_DIR}/include
						${Behaviour_DIR}/include/behavior
						${Behaviour_DIR}/behavior/include
						${Behaviour_DIR}/include/${behaviorComp}
						${Behaviour_DIR}/${behaviorComp}/include
						${${behaviorCompUC}_DIR}/include
						${${behaviorCompUC}_DIR}/${behaviorComp}/include
						${${behaviorCompUC}_DIR}
		)
		## check if found
		if(NOT Behaviour_${behaviorCompUC}_INCLUDE_DIR)
			message(WARNING "   Failed to find ${behaviorComp} :\nBehaviour_${behaviorCompUC}_INCLUDE_DIR not found.\nCheck you write correctly the component name (case sensitive),\nor set the Behaviour_${behaviorCompUC}_DIR to look inside")
		else()
			list(APPEND Behaviour_INCLUDE_DIRS	${Behaviour_${behaviorCompUC}_INCLUDE_DIR})
		endif()

		## try to find filepath lib name (looking for very important lib file)
		find_library(Behaviour_${behaviorCompUC}_LIBRARY_RELEASE 
			NAMES 			${behaviorComp}_vc110_x${Behaviour_SEARCH_LIB_POSTFIX} 		${behaviorCompLC}_vc110_x${Behaviour_SEARCH_LIB_POSTFIX}  	${behaviorCompUC}_vc110_x${Behaviour_SEARCH_LIB_POSTFIX} 
			PATHS 			/opt/local/lib		
							/usr/lib
							/usr/local/lib
							${Behaviour_DIR}/lib
							${${behaviorCompUC}_DIR}/lib
							${${behaviorCompUC}_DIR}
			PATH_SUFFIXES	Release
		)
		#message(WARNING "Search ${behaviorComp}_vc110_x${Behaviour_SEARCH_LIB_POSTFIX}_d")
		find_library(Behaviour_${behaviorCompUC}_LIBRARY_DEBUG 
			NAMES 			${behaviorComp}_vc110_x${Behaviour_SEARCH_LIB_POSTFIX}_d 		${behaviorCompLC}_vc110_x${Behaviour_SEARCH_LIB_POSTFIX}_d  	${behaviorCompUC}_vc110_x${Behaviour_SEARCH_LIB_POSTFIX}_d
			PATHS 			/opt/local/lib 		
							/usr/lib
							/usr/local/lib
							${Behaviour_DIR}/lib
							${${behaviorCompUC}_DIR}/lib
							${${behaviorCompUC}_DIR}
			PATH_SUFFIXES	Debug
		)
		
		## check and auto complete release with debug if release missing and vice versa
		
		if(Behaviour_${behaviorCompUC}_LIBRARY_RELEASE)
			if(NOT Behaviour_${behaviorCompUC}_LIBRARY_DEBUG)
				set(Behaviour_${behaviorCompUC}_LIBRARY_DEBUG ${Behaviour_${behaviorCompUC}_LIBRARY_RELEASE} CACHE PATH "Path to a library." FORCE)
			endif()
		endif()
		if(Behaviour_${behaviorCompUC}_LIBRARY_DEBUG)
			if(NOT Behaviour_${behaviorCompUC}_LIBRARY_RELEASE)
				set(Behaviour_${behaviorCompUC}_LIBRARY_RELEASE ${Behaviour_${behaviorCompUC}_LIBRARY_DEBUG} CACHE PATH "Path to a library." FORCE)
			endif()
		endif()
		
		## check and append the and Behaviour_LIBRARIES list, and warn if not found (release and debug) otherwise
		if(NOT Behaviour_${behaviorCompUC}_LIBRARY_RELEASE AND NOT Behaviour_${behaviorCompUC}_LIBRARY_DEBUG)
			message(WARNING "   Failed to find ${behaviorComp} :
			Check you write correctly the component name (case sensitive),
			or set the Behaviour_${behaviorCompUC}_DIR to look inside,
			or set directly Behaviour_${behaviorCompUC}_LIBRARY_DEBUG and Behaviour_${behaviorCompUC}_LIBRARY_RELEASE
			")
		else()
			list(APPEND Behaviour_LIBRARIES	optimized "${Behaviour_${behaviorCompUC}_LIBRARY_RELEASE}" debug "${Behaviour_${behaviorCompUC}_LIBRARY_DEBUG}")
		endif()
		
		## here we allow to find at least the include OR the lib dir and just warn if one of both missing
		if(NOT Behaviour_${behaviorCompUC}_INCLUDE_DIR AND NOT Behaviour_${behaviorCompUC}_LIBRARY_RELEASE)
			set(Behaviour_${behaviorCompUC}_FOUND OFF)
		else()
			set(Behaviour_${behaviorCompUC}_FOUND ON)
		endif()
		
		## if one of both (include dir or filepath lib), then we provide a new cmake cache variable for the search. Otherwise we don't need anymore to expose all intermediates variables
		if(NOT Behaviour_${behaviorCompUC}_FOUND)
			set(Behaviour_${behaviorCompUC}_DIR "$ENV{Behaviour_${behaviorCompUC}_DIR}" CACHE PATH "${behaviorComp} root directory")
		else()
			mark_as_advanced(Behaviour_${behaviorCompUC}_INCLUDE_DIR)
			mark_as_advanced(Behaviour_${behaviorCompUC}_LIBRARY_RELEASE)
			mark_as_advanced(Behaviour_${behaviorCompUC}_LIBRARY_DEBUG)
			if(DEFINED Behaviour_${behaviorCompUC}_DIR)
				mark_as_advanced(Behaviour_${behaviorCompUC}_DIR)
			endif()
		endif()

		if(Behaviour_VERBOSE)
			message(STATUS "   Behaviour_${behaviorCompUC}_FOUND = ${Behaviour_${behaviorCompUC}_FOUND} : ")
			message(STATUS "      * Behaviour_${behaviorCompUC}_INCLUDE_DIR = ${Behaviour_${behaviorCompUC}_INCLUDE_DIR}")
			message(STATUS "      * Behaviour_${behaviorCompUC}_LIBRARY_DEBUG = ${Behaviour_${behaviorCompUC}_LIBRARY_DEBUG}")
			message(STATUS "      * Behaviour_${behaviorCompUC}_LIBRARY_RELEASE = ${Behaviour_${behaviorCompUC}_LIBRARY_RELEASE}")
		endif()
		
		list(APPEND Behaviour_FOUND_LIST Behaviour_${behaviorCompUC}_FOUND)
		
	endforeach()
	
	
	## set the final Behaviour_FOUND based on all previous components found (status)
	foreach(componentToCheck ${Behaviour_FOUND_LIST})
		set(Behaviour_FOUND ON)
		if(NOT ${componentToCheck})
			set(Behaviour_FOUND OFF)
			break() ## one component not found is enought to failed
		endif()
	endforeach()
endmacro()

## Default behavior if user don't use the COMPONENTS flag in find_package(Behaviour ...) command
if(NOT Behaviour_FIND_COMPONENTS)
	list(APPEND Behaviour_FIND_COMPONENTS MicroSimUtilities SituatedAgent TopoEnvironment TopoEnvSimpleLoader UnifiedLOD VirtualHuman)
endif()

Behaviour_FIND_COMPONENTS()
if(Behaviour_DIR)
	check_cached_var(Behaviour_INCLUDE 	"${Behaviour_DIR}/include" PATH "")
endif()

if(Behaviour_INCLUDE_DIRS)
	list(REMOVE_DUPLICATES Behaviour_INCLUDE_DIRS)
endif()
if(Behaviour_LIBRARIES)
	list(REMOVE_DUPLICATES Behaviour_LIBRARIES)
endif()

if(Behaviour_VERBOSE)
	message(STATUS "Finish to FindBehaviour.cmake => Behaviour_FOUND=${Behaviour_FOUND}")
endif()
