cmake_minimum_required(VERSION 3.11)

set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}")

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build, options are: Debug, Release, or MinSizeRel." FORCE)
    message(STATUS "CMAKE_BUILD_TYPE not set, defaulting to MinSizeRel.")
endif()

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${CMAKE_SOURCE_DIR}/cmake")

if(MSVC)
    set(CMAKE_DEBUG_POSTFIX d)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MP")  # enable parallel builds
endif()
	
message(STATUS "Generator .............. ${CMAKE_GENERATOR}")
message(STATUS "Build Type ............. ${CMAKE_BUILD_TYPE}")

project(RLTest)

set (SOURCE_FILE 	        
			./include/main.h
			#./include/stb_image.h

			./src/Camera.cpp
			./src/timestep.cpp
			
		   )

add_executable(RLTestBin 
	        ${SOURCE_FILE}
			./src/main.cpp		
			)

set_source_files_properties( 
	        ${SOURCE_FILE}
			PROPERTIES HEADER_FILE_ONLY TRUE)


set_target_properties( RLTestBin
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
	VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}" 
	WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}" 
)

include(build_options)
include(build_dependencies)

target_include_directories(RLTestBin
					PRIVATE
					include
					${SDL2_INCLUDE_DIR}
					${SFML_INCLUDE_DIR}
				)
				
target_link_directories(
			RLTestBin
			PRIVATE
			${SDL2_LIBRARY_DIR}
			${SFML_LIBRARY_DIR}
			)
			
target_link_libraries(RLTestBin 
			${SDL2_LIBRARIES}
			${SFML_LIBRARIES}
			)
			
add_custom_command(TARGET RLTestBin POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_INSTALL_PREFIX}/ressources"
       "${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}")

#add_custom_command(TARGET RLTestBin POST_BUILD
#    COMMAND  signtool sign  /debug /a $<TARGET_FILE:RLTestBin>)
			

	   
	   
	   