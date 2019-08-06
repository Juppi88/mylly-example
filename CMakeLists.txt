####################################################################################################
# 
# This is the CMake file for the Mylly example game.
# The recommended way to run this is in a separate build directory, e.g.
# 
# mkdir build && cd build
# cmake ..
# 
####################################################################################################

cmake_minimum_required(VERSION 3.0)
project(MyllyExample VERSION 0.0.1 LANGUAGES CXX)

# Custom output location.
if (NOT DEFINED OUTPUT_DIR)
	set(OUTPUT_DIR ${CMAKE_BINARY_DIR}/output)
endif (NOT DEFINED OUTPUT_DIR)

file(MAKE_DIRECTORY ${OUTPUT_DIR})
set(EXECUTABLE_OUTPUT_PATH ${OUTPUT_DIR})
file(REMOVE ${OUTPUT_DIR})

# Define compiler flags.
set(CMAKE_CXX_FLAGS         "-std=c++11 -DCGLM_ALL_UNALIGNED")
set(CMAKE_CXX_FLAGS_DEBUG   "-D_DEBUG -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O4 -DNDEBUG")

# Additional flags for GCC/clang.
if (UNIX)
	set(CMAKE_CXX_FLAGS       "${CMAKE_CXX_FLAGS} -Wall -Wextra")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
endif ()

if (MSVC)
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Zi")
endif ()

# Add the CMake scripts for the engine library and editor utilities.
add_subdirectory("mylly/mylly")
add_subdirectory("editor")

# Define source files for the game.
file(GLOB EXAMPLE_SRC
    "*.h"
    "*.cpp"
)

# Create an executable from the source.
add_executable(game ${EXAMPLE_SRC})
target_link_libraries(game mylly_editor mylly)

# Compiler-specific flags.
if (MSVC)
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SAFESEH:NO")

	# Visual Studio appends either Debug/ or Release/ to the build path based on configuration.
	if (CMAKE_BUILD_TYPE EQUAL "DEBUG")
		set(OUTPUT_DIR ${OUTPUT_DIR}/Debug)
	else ()
		set(OUTPUT_DIR ${OUTPUT_DIR}/Release)
	endif ()


	# Copy dependency libraries to build path.
	set(EXT_LIB_PATH "${CMAKE_CURRENT_SOURCE_DIR}/mylly/external")

	file(COPY ${EXT_LIB_PATH}/freetype/bin/freetype.dll DESTINATION ${OUTPUT_DIR})
	file(COPY ${EXT_LIB_PATH}/jpeg/bin/jpeg62.dll DESTINATION ${OUTPUT_DIR})
	file(COPY ${EXT_LIB_PATH}/libpng/bin/libpng3.dll DESTINATION ${OUTPUT_DIR})
	file(COPY ${EXT_LIB_PATH}/libpng/bin/libpng12.dll DESTINATION ${OUTPUT_DIR})
	file(COPY ${EXT_LIB_PATH}/openal/bin/Win32/soft_oal.dll DESTINATION ${OUTPUT_DIR})
	file(COPY ${EXT_LIB_PATH}/zlib/bin/zlib1.dll DESTINATION ${OUTPUT_DIR})
endif (MSVC)

# Copy the game's resource files to the output directory.

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/mylly/mylly/resources/shaders DESTINATION ${OUTPUT_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/editor/resources/textures DESTINATION ${OUTPUT_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/editor/resources/fonts DESTINATION ${OUTPUT_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources/effects DESTINATION ${OUTPUT_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources/fonts DESTINATION ${OUTPUT_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources/models DESTINATION ${OUTPUT_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources/shaders DESTINATION ${OUTPUT_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources/sounds DESTINATION ${OUTPUT_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources/textures DESTINATION ${OUTPUT_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources/credits.txt DESTINATION ${OUTPUT_DIR})
