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
set(CMAKE_CXX_FLAGS         "-Wall -Wextra -std=c++11 -DCGLM_ALL_UNALIGNED")
set(CMAKE_CXX_FLAGS_DEBUG   "-g -D_DEBUG -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O4 -DNDEBUG")

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

# Copy the game's resource files to the output directory.
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/mylly/mylly/resources/shaders DESTINATION ${OUTPUT_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources/effects DESTINATION ${OUTPUT_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources/fonts DESTINATION ${OUTPUT_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources/models DESTINATION ${OUTPUT_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources/shaders DESTINATION ${OUTPUT_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources/textures DESTINATION ${OUTPUT_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/editor/resources/textures DESTINATION ${OUTPUT_DIR})
