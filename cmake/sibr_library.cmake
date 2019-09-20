# NOTE
# This feature is used to easily download, store and link external dependencies. This
# requires to prepare pre-compiled libraries (to download). For now, packages have
# only be prepare for Windows 64-bit with Visual Studio 2012. (You should re-build
# everything if you want to use another version of Visual Studio/ another compiler).

# NOTE ABOUT UNIX SYSTEMS
# There is no need for "searching mechanism". This function is discard and your
# libraries should be installed is the standard folders that are:
#
# /usr/include/
# /usr/lib/
# /usr/lib64/
# for packages downloaded using apt-get/yum
# 
# /usr/local/include/
# /usr/local/lib/
# /usr/local/lib64/
# for packages manually installed ("make install")
#
# if you encounter problems when linking (e.g. lib not found even if it is installed),
# please check these folders are in your search PATH environment variables.



function(sibr_addlibrary)
    if(NOT WIN32)
        return()
    endif()

    if (MSVC11 OR MSVC12)
        set(EXTLIBS_PACKAGE_FOLDER "${CMAKE_SOURCE_DIR}/extlibs/win64-msvc2012/")
    elseif(MSVC14)
        ## TODO SV: build these libraries and put them in /extlibs/win64-msvc2017
        # right now the only changed from MSVC2012 is glfw (using VS 2015 binary)
        set(EXTLIBS_PACKAGE_FOLDER "${CMAKE_SOURCE_DIR}/extlibs/win64-msvc2017/")
    else()
        message(WARNING "There is no precompiled library supporting this version of MSVC")
    endif()

    file(MAKE_DIRECTORY ${EXTLIBS_PACKAGE_FOLDER})

    include(CMakeParseArguments)
    cmake_parse_arguments(args "" "NAME;VERSION;MSVC11;MSVC12;MSVC14" "REQUIREDFOR" ${ARGN})


    if (NOT "${args_VERSION}" MATCHES "")
        message(WARNING "VERSION is not implemented yet")
    endif()

    set(lcname "")
    set(ucname "")
    string(TOLOWER "${args_NAME}" lcname)
    string(TOUPPER "${args_NAME}" ucname)


    # message("${args_NAME} required for: ${args_REQUIREDFOR}")
    if (${args_REQUIREDFOR} MATCHES "")
        # message("${args_REQUIREDFOR} matches the regular expresion <empty>")
        message(STATUS "Adding library '${args_NAME}'")
    else()
        # message("${args_REQUIREDFOR} doesnt matches the regular expresion <empty>")
        set(used FALSE)
        foreach(target ${args_REQUIREDFOR})
            #message(STATUS "----> ${target}")
            if(${target})
                set(used TRUE)
            endif()
        endforeach()

        if (used)
            message(STATUS "Adding library '${args_NAME}' required for ${args_REQUIREDFOR}")
        else()
            return()
        endif()
    endif()


    set(LIB_PACKAGE_FOLDER "${EXTLIBS_PACKAGE_FOLDER}/${lcname}")
    win3rdParty(${ucname} #VERBOSE ON
                    MSVC11 "${LIB_PACKAGE_FOLDER}" "${args_MSVC11}"
                    MSVC12 "${LIB_PACKAGE_FOLDER}" "${args_MSVC12}"
                    MSVC14 "${LIB_PACKAGE_FOLDER}" "${args_MSVC14}" # TODO SV: make sure to build this library if required
                )
			
    # Add include/ directory
    # and lib/ directories

    # TODO SV: paths not matching with current hierarchy. example: libraw/libraw-0.17.1/include
    # SR:	The link directories will also be used to lookup for dependency DLLs to copy in the install directory.
    #		Some libraries put the DLLs in the bin/ directory, so we include those.
    file(GLOB subdirs RELATIVE ${LIB_PACKAGE_FOLDER} ${LIB_PACKAGE_FOLDER}/*)
    set(dirlist "")
    foreach(dir ${subdirs})
        # message("adding ${LIB_PACKAGE_FOLDER}/${dir}/include/ to the include directories")
        include_directories("${LIB_PACKAGE_FOLDER}/${dir}/include/")
        # message("adding ${LIB_PACKAGE_FOLDER}/${dir}/lib[64] to the link directories")
        link_directories("${LIB_PACKAGE_FOLDER}/${dir}/lib/")
        link_directories("${LIB_PACKAGE_FOLDER}/${dir}/lib64/")
        link_directories("${LIB_PACKAGE_FOLDER}/${dir}/bin/")
    endforeach()

endfunction()


## OLD VERSION
## sibr_addlibrary( NAME <libname> VERSION <x.x.x> )
#function(sibr_addlibrary)
#  if(NOT WIN32)
#      return()
#  endif()
#
#  set(EXTLIBS_PACKAGE_FOLDER "${CMAKE_SOURCE_DIR}/extlibs/win64-msvc2012/")
#  file(MAKE_DIRECTORY ${EXTLIBS_PACKAGE_FOLDER})
#
#  include(CMakeParseArguments)
#  cmake_parse_arguments(args "" "NAME;VERSION;MSVC11;MSVC12" "REQUIREDFOR" ${ARGN})
#
#
#  if (NOT "${args_VERSION}" MATCHES "")
#    message(WARNING "VERSION is not implemented yet")
#  endif()
#
#  set(lcname "")
#  set(ucname "")
#  string(TOLOWER "${args_NAME}" lcname)
#  string(TOUPPER "${args_NAME}" ucname)
#
#
#  if (${args_REQUIREDFOR} MATCHES "")
#    message(STATUS "Adding library '${args_NAME}'")
#  else()
#    set(used FALSE)
#    foreach(target ${args_REQUIREDFOR})
#      #message(STATUS "----> ${target}")
#      if(${target})
#        set(used TRUE)
#      endif()
#    endforeach()
#
#    if (used)
#      message(STATUS "Adding library '${args_NAME}' required for ${args_REQUIREDFOR}")
#    else()
#      return()
#    endif()
#  endif()
#
#
#  set(LIB_PACKAGE_FOLDER "${EXTLIBS_PACKAGE_FOLDER}/${lcname}")
#	win3rdParty(${ucname} #VERBOSE ON
#        	MSVC11 "${LIB_PACKAGE_FOLDER}" "${args_MSVC11}"
#          MSVC12 "${LIB_PACKAGE_FOLDER}" "${args_MSVC12}"
#			)
#
#  # Search for the root directory of the given library name
#  file(GLOB subdirs RELATIVE ${LIB_PACKAGE_FOLDER} ${LIB_PACKAGE_FOLDER}/*)
#  set(dirlist "")
#  foreach(dir ${subdirs})
#    set(lcdir "")
#    string(TOLOWER "${dir}" lcdir)
#    if(IS_DIRECTORY ${LIB_PACKAGE_FOLDER}/${dir})
#    set(foundpos -1)
#    string(FIND "${lcdir}" "${lcname}" foundpos)
#      if(foundpos EQUAL 0)
#          LIST(APPEND dirlist ${dir})
#          LIST(APPEND dirlist ${dir})
#      endif()
#    endif()
#  endforeach()
#
#  set(dirlistlen 0)
#  list(LENGTH dirlist dirlistlen)
#  if (dirlistlen EQUAL 0)
#    message(FATAL_ERROR "library '${lcname}' is not found in '${LIB_PACKAGE_FOLDER}'")
#  endif()
#
#  set(libfolder "")
#  list(GET dirlist 0 libfolder)
#  set(libfolder "${LIB_PACKAGE_FOLDER}/${libfolder}")
#
#  set("${ucname}_DIR" ${libfolder} CACHE STRING "")
#  set("${ucname}_INCLUDE_DIR" ${libfolder}/include CACHE STRING "")
#  set("${ucname}_LIB_DIR" ${libfolder}/lib CACHE STRING "")
#    # set("${ucname}_LIBRARIES" ${libfolder} CACHE STRING "")
#    # set("${ucname}_FOUND" FALSE)
#
#  include_directories("${${ucname}_INCLUDE_DIR}")
#  link_directories("${${ucname}_LIB_DIR}")
#  # IN PROGRESS !!!
#
#  #Search for library themselve
#
#  #set("${ucname}_DIR" ${libfolder} CACHE STRING "")
#  #set("${ucname}_INCLUDE_DIR" ${libfolder}/include CACHE STRING "")
#  #set("${ucname}_LIBRARIES" ${libfolder} CACHE STRING "")
#  #set("${ucname}_FOUND" FALSE)
#
#endfunction()
#
