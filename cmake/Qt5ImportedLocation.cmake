if(NOT WIN32 OR __Qt5ImportedLocation_cmake_INCLUDED__)
	return()
else()
	set(__Qt5ImportedLocation_cmake_INCLUDED__ ON)
endif()


## Use Internaly for Qt5ImportedLocation function
##
## look for all required filePath modules (components and plugins Qt5 targets)
##
##	Written by Jerome Esnault
function(fillQt5ImportedLocationList configUC returnedList)
	include(CMakeParseArguments)
	cmake_parse_arguments(qfill "" "" "QT5_MODULES" ${ARGN} )
	foreach(qtComp ${qfill_QT5_MODULES})
		get_target_property(${qtComp}_targ_imp_loc ${qtComp} IMPORTED_LOCATION_${configUC})
		if(${qtComp}_targ_imp_loc)
			if(EXISTS ${${qtComp}_targ_imp_loc})
				list(APPEND moduleList ${${qtComp}_targ_imp_loc})
			else()
				message(WARNING "IMPORTED_LOCATION_${configUC} of ${qtComp} doesn't exist.")
			endif()
		else()
			message(WARNING "IMPORTED_LOCATION_${configUC} of ${qtComp} is empty...")
		endif()
	endforeach()
	if(NOT moduleList)
		set(moduleList )
	endif()
	set(${returnedList} ${moduleList} PARENT_SCOPE)
endfunction()


##	
##	Find where dynamique libraries are stored (return the absolute QT5 BINARY DIR)
##
##	Find list of dynamique libraries absolute filePathName (*.dll on windows or *.so on UNIX)
##      Available qt5 components are easi to find as they are named like libraries name (without Qt5 prefix and confiMode suffix)
##
##	Find list of plugins absolute filePathName (*.dll on windows or *.so on UNIX)
##      Available qt5 plugins are (Search "_populate_[1-2]*[a-z]*[A-z]*_plugin_properties" in cmake dir of Qt5) :
##      AxBase plugins :             
##      AxContainer plugins :       
##      AxServer plugins :          
##      Bluetooth plugins :         
##      Concurrent plugins :        
##      Core plugins :              
##      Declarative plugins :       QTcpServerConnection QtQuick1Plugin
##      Designer plugins :          QAxWidgetPlugin QDeclarativeViewPlugin QQuickWidgetPlugin
##      Gui plugins :               QDDSPlugin  QGifPlugin                  QICNSPlugin QICOPlugin                  QJp2Plugin 
##                                  QJpegPlugin QMinimalIntegrationPlugin   QMngPlugin  QOffscreenIntegrationPlugin QTgaPlugin
##                                  QTiffPlugin QWbmpPlugin                 QWebpPlugin QWindowsIntegrationPlugin
##      Help plugins :              
##      Location plugins :          QGeoServiceProviderFactoryNokia QGeoServiceProviderFactoryOsm
##      MultiMedia plugins:         AudioCaptureServicePlugin DSServicePlugin QM3uPlaylistPlugin QWindowsAudioPlugin WMFServicePlugin
##      MultiMediaWidgets plugins:
##      Network plugins :           QGenericEnginePlugin QNativeWifiEnginePlugin
##      Nfc plugins :               
##      OpenGL plugins :            
##      OpenGLExtensions plugins :  
##      Positioning plugins :       QGeoPositionInfoSourceFactoryPoll
##      PrintSupport plugins :      QWindowsPrinterSupportPlugin
##      Qml plugins :               QTcpServerConnection QtQuick2Plugin
##      Quick plugins :             
##      QuickTest plugins :         
##      QuickWidgets plugins :      
##      Script plugins :            
##      ScriptTools plugins :       
##      Sensors plugins :           dummySensorPlugin genericSensorPlugin QShakeSensorGesturePlugin QtSensorGesturePlugin
##      SerialPort plugins :        
##      SQL plugins :               QSQLiteDriverPlugin
##      SVG plugins :               QSvgIconPlugin QSvgPlugin
##      Test plugins :              
##      UiTools plugins :           
##      WebChannel plugins :        
##      WebSockets plugins :        
##      Widgets plugins :           
##      WinExtras plugins :         
##      XML plugins :               
##      XMLPatterns plugins :       
## 
##Usage examples:
##	Qt5ImportedLocation( QT5_DYN_LIB_DIR QT5_DYN_LIBS QT5_PLUGINS) 	## default Release with Qt5 Core and associated dependencies
##	#<OR>
##	Qt5ImportedLocation( QT5_DYN_LIB_DIR QT5_DYN_LIBS QT5_PLUGINS
##		CONFIG_MODE 	Release Debug
##		QT5_COMPONENTS	Qt5::Core Qt5::Gui Qt5::Widgets Qt5::OpenGL
##      QT5_PLUGINS     Qt5::QWindowsIntegrationPlugin ## or ${Qt5Gui_PLUGINS} , Qt5<Module>_PLUGINS
##	)
##	message(QT5_DYN_LIB_DIR = ${QT_BINARY_DIR})
##	foreach(lib ${QT5_DYN_LIBS} ${QT5_PLUGINS})
##		message(${lib})
##	endforeach()
##	
##	Written by Jerome Esnault	
function(Qt5ImportedLocation returnQt5DynDir returnedDynLibsList returnedPluginsList) ## configMode support only Debug or Release
    include(CMakeParseArguments)
	cmake_parse_arguments(qtil "" "" "CONFIG_MODE;QT5_COMPONENTS;QT5_PLUGINS" ${ARGN} )
	if(NOT qtil_CONFIG_MODE)
		if(DEFINED CMAKE_BUILD_TYPE)
			set(qtil_CONFIG_MODE ${CMAKE_BUILD_TYPE})
		else()
			set(qtil_CONFIG_MODE Release Debug)
		endif()
	endif()
	set(Qt5_DYNLIB_DIR 	"")
	set(Qt5_dynLibs		"")
    set(Qt5_plugins		"")
    
    ## default results (in any case there is the Qt5_DYNLIB_DIR and maybe some extras needed dynLibs/plugins)
    foreach(config ${qtil_CONFIG_MODE})
		string(TOUPPER ${config} config_UC)
        get_target_property(Qt5Core_targ_imp_loc Qt5::Core IMPORTED_LOCATION_${config_UC})
        if(Qt5Core_targ_imp_loc)
            get_filename_component(Qt5_DYNLIB_PATH ${Qt5Core_targ_imp_loc} PATH)
            file(GLOB 	Qt5_UCU_DYNLIB_FILES "${Qt5_DYNLIB_PATH}/icu*${CMAKE_SHARED_LIBRARY_SUFFIX}") ## for Qt <= 5.3
            list(APPEND Qt5_dynLibs ${Qt5_UCU_DYNLIB_FILES} ${Qt5Core_targ_imp_loc})
            set(Qt5_DYNLIB_DIR ${Qt5_DYNLIB_PATH}) ## we know debug and release targets are in the same place (this value will not change)
        elseif()
            message(WARNING "Cannot find Qt5::Core IMPORTED_LOCATION_${config_UC}. Qt5_DYNLIB_DIR=${Qt5_DYNLIB_DIR}")
        endif()
	endforeach()
	if(WIN32)
		foreach(config RELEASE DEBUG)
			get_target_property(Qt5WindowsPlugin_targ_imp_loc Qt5::QWindowsIntegrationPlugin IMPORTED_LOCATION_${config})
			if(Qt5WindowsPlugin_targ_imp_loc)
				list(APPEND Qt5_plugins ${Qt5WindowsPlugin_targ_imp_loc})
			else()
				message(WARNING "Cannot find Qt5::QWindowsIntegrationPlugin IMPORTED_LOCATION_${config}.")
			endif()
		endforeach()
	endif()
    
	foreach(config ${qtil_CONFIG_MODE})
		string(TOUPPER ${config} config_UC)
        if(qtil_QT5_COMPONENTS)
            fillQt5ImportedLocationList(${config_UC} Qt5_Comps_dynLibs QT5_MODULES ${qtil_QT5_COMPONENTS} )
            list(APPEND Qt5_dynLibs ${Qt5_Comps_dynLibs})
        endif()
        if(qtil_QT5_PLUGINS)
            fillQt5ImportedLocationList(${config_UC} Qt5_plugins_dynLibs QT5_MODULES ${qtil_QT5_PLUGINS} )
            list(APPEND Qt5_plugins ${Qt5_plugins_dynLibs})
        endif()
	endforeach()

	list(REMOVE_DUPLICATES Qt5_dynLibs)
    list(REMOVE_DUPLICATES Qt5_plugins)
	set(${returnedDynLibsList} 	${Qt5_dynLibs} 		PARENT_SCOPE)
	set(${returnQt5DynDir} 		${Qt5_DYNLIB_DIR} 	PARENT_SCOPE)
    set(${returnedPluginsList} 	${Qt5_plugins} 	    PARENT_SCOPE)
endfunction()