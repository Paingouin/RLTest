#Commande lline exemple :> cmake . -B ./build


cmake_minimum_required(VERSION 3.12)

set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}")

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build, options are: Debug, Release, or MinSizeRel." FORCE)
    message(STATUS "CMAKE_BUILD_TYPE not set, defaulting to Debug.")
endif()

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${CMAKE_SOURCE_DIR}/cmake")

if(MSVC)
    set(CMAKE_DEBUG_POSTFIX d)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MP")  # enable parallel builds
endif()
	
message(STATUS "Generator .............. ${CMAKE_GENERATOR}")
message(STATUS "Build Type ............. ${CMAKE_BUILD_TYPE}")


project(RLTest)

set (SOURCE_FILE_PLATEFORM 	        
			./include/main.h
			#./include/stb_image.h

			./src/Camera.cpp
			./src/timestep.cpp
			
		   )

set_source_files_properties( 
	        ${SOURCE_FILE_PLATEFORM}
			PROPERTIES HEADER_FILE_ONLY TRUE)



add_executable(RLTestBin 
	        ${SOURCE_FILE_PLATEFORM}
			./src/main.cpp		
			)

set_target_properties( RLTestBin
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}"
	VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}" 
	WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}" 
)

# /!\ need this for windows !!!
file(TO_CMAKE_PATH "${CMAKE_INSTALL_PREFIX}" CMAKE_INSTALL_PREFIX)

INCLUDE(ExternalProject)

IF(NOT WIN32)
	set(FLAGS_FOR_DYNAMIC_LINK -fPIC) 
ENDIF()

	SET(SFML_RELEASE 2.5.1)
ExternalProject_Add(sfml_ext
	URL https://www.sfml-dev.org/files/SFML-${SFML_RELEASE}-sources.zip
	URL_HASH "SHA256=bf1e0643acb92369b24572b703473af60bac82caf5af61e77c063b779471bb7f"
	UPDATE_COMMAND ""
	DOWNLOAD_DIR ${CMAKE_BINARY_DIR}
	SOURCE_DIR ${CMAKE_BINARY_DIR}/SFML-${SFML_RELEASE}
	BUILD_IN_SOURCE 0
	
	CMAKE_ARGS
		${SDL_CMAKE_BUILD_OPTS}
		-DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}/deps/${CMAKE_BUILD_TYPE}
		-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
		-DCMAKE_C_FLAGS=${FLAGS_FOR_DYNAMIC_LINK} ${EXTERNAL_C_FLAGS}
	)
	
add_library(sfml STATIC IMPORTED GLOBAL)
add_dependencies(sfml sfml_ext)
	
MESSAGE(STATUS "SFML Installing to: ${CMAKE_INSTALL_PREFIX}/${CMAKE_BUILD_TYPE}")
SET(SFML_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/deps/${CMAKE_BUILD_TYPE}/include)
SET(SFML_LIBRARY_DIR ${CMAKE_INSTALL_PREFIX}/deps/${CMAKE_BUILD_TYPE}/lib)

message("${SFML_LIBRARY_DIR}")

set(SFML_GRAPHIC_BASE_NAME "sfml-graphics")
set(SFML_WINDOW_BASE_NAME "sfml-window")
set(SFML_SYSTEM_BASE_NAME "sfml-system")

IF("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
	IF(WIN32)
		 set(ARCH_PREFIX "${CMAKE_STATIC_LIBRARY_PREFIX}" )
		 set(ARCH_POSTFIX "-d${CMAKE_STATIC_LIBRARY_SUFFIX}")
	ELSEIF(APPLE)
		 set(ARCH_POSTFIX "")
	 ELSE()
		 set(ARCH_PREFIX "" )
		 set(ARCH_POSTFIX "-d")
	ENDIF()
ELSE()
	IF(WIN32)
		 set(ARCH_POSTFIX "")
	ELSEIF(APPLE)
		 set(ARCH_POSTFIX "")
	ELSE()
		 set(ARCH_POSTFIX "")
	ENDIF()
ENDIF()
set(SFML_GRAPHIC_NAME
"${ARCH_PREFIX}${SFML_GRAPHIC_BASE_NAME}${ARCH_POSTFIX}")
set(SFML_WINDOW_NAME
"${ARCH_PREFIX}${SFML_WINDOW_BASE_NAME}${ARCH_POSTFIX}")
set(SFML_SYSTEM_NAME
"${ARCH_PREFIX}${SFML_SYSTEM_BASE_NAME}${ARCH_POSTFIX}")

set(SFML_LIBRARIES "${SFML_GRAPHIC_NAME};${SFML_WINDOW_NAME};${SFML_SYSTEM_NAME}" )
message("${SFML_LIBRARIES}")

add_dependencies(RLTestBin sfml)

add_custom_command(TARGET sfml_ext POST_BUILD
COMMAND ${CMAKE_COMMAND} -E copy_directory
	"${CMAKE_INSTALL_PREFIX}/deps/${CMAKE_BUILD_TYPE}/bin"
   "${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}")

target_include_directories(RLTestBin
					PRIVATE
					include
					#${SDL2_INCLUDE_DIR}
					${SFML_INCLUDE_DIR}
				)
				
target_link_directories(
			RLTestBin
			PRIVATE
			#${SDL2_LIBRARY_DIR}
			${SFML_LIBRARY_DIR}
			)
	
target_link_libraries(RLTestBin 
			#${SDL2_LIBRARIES}
			${SFML_LIBRARIES}
			)
			

#copy the ressources folder to the bin folder
add_custom_command(TARGET RLTestBin POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_INSTALL_PREFIX}/ressources"
       "${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}")

#add_custom_command(TARGET RLTestBin POST_BUILD
#    COMMAND  signtool sign  /debug /a $<TARGET_FILE:RLTestBin>)
			

	   
	   
	   