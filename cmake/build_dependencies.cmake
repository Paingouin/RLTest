
# IF (PDC_SDL2_DEPS_BUILD)
    INCLUDE(ExternalProject)

    IF(NOT WIN32)
        set(FLAGS_FOR_DYNAMIC_LINK -fPIC) 
    ENDIF()

    SET(SDL2_RELEASE 2.0.10)
    ExternalProject_Add(sdl2_ext
        URL https://www.libsdl.org/release/SDL2-${SDL2_RELEASE}.zip
        URL_HASH "SHA256=658b0435f57d496e967c1996badbd83bac120689a693f57c4007698d0fe24543"
        UPDATE_COMMAND ""
        DOWNLOAD_DIR ${CMAKE_BINARY_DIR}
        SOURCE_DIR ${CMAKE_BINARY_DIR}/SDL2-${SDL2_RELEASE}
        BUILD_IN_SOURCE 0
        CMAKE_ARGS
            ${SDL_CMAKE_BUILD_OPTS}
            -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}/deps/${CMAKE_BUILD_TYPE}
            -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
            -DCMAKE_C_FLAGS=${FLAGS_FOR_DYNAMIC_LINK} ${EXTERNAL_C_FLAGS}
			-DCMAKE_RUNTIME_OUTPUT_DIRECTORY=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        )
		
	add_library(sdl2 STATIC IMPORTED GLOBAL)
	add_dependencies(sdl2 sdl2_ext)

    MESSAGE(STATUS "SDL2 Installing to: ${CMAKE_INSTALL_PREFIX}/${CMAKE_BUILD_TYPE}")
    SET(SDL2_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/deps/${CMAKE_BUILD_TYPE}/include/SDL2)
    SET(SDL2_LIBRARY_DIR ${CMAKE_INSTALL_PREFIX}/deps/${CMAKE_BUILD_TYPE}/lib)
    IF("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
        IF(WIN32)
            set(SDL2_LIBRARIES "SDL2maind.lib;SDL2d.lib")
            set(SDL2_LIBRARY "SDL2d.lib")
        ELSEIF(APPLE)
            set(SDL2_LIBRARIES "SDL2maind;SDL2d")
            set(SDL2_LIBRARY "SDL2d")
        ELSE()
            set(SDL2_LIBRARIES "SDL2maind;SDL2-2.0d")
            set(SDL2_LIBRARY "SDL2-2.0d")
        ENDIF()
    ELSE()
        IF(WIN32)
            set(SDL2_LIBRARIES "SDL2main.lib;SDL2.lib")
            set(SDL2_LIBRARY "SDL2.lib")
        ELSEIF(APPLE)
            set(SDL2_LIBRARIES "SDL2main;SDL2")
            set(SDL2_LIBRARY "SDL2")
        ELSE()
            set(SDL2_LIBRARIES "SDL2main;SDL2-2.0")
            set(SDL2_LIBRARY "SDL2-2.0")
        ENDIF()
    ENDIF()
	
# ENDIF()