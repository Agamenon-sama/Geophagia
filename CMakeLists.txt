cmake_minimum_required(VERSION 4.0) # might need less
project("Geophagia" VERSION 0.1)

# Settings
set(CMAKE_CXX_STANDARD 23) # use C++23
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE) # export compile_commands.json
set(THREADS_PREFER_PTHREAD_FLAG ON) # use pthreads if available

# Warnings
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /WX")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
endif()

# Add src files
file(GLOB_RECURSE SRC_FILES
    ${CMAKE_SOURCE_DIR}/src/*.c
    ${CMAKE_SOURCE_DIR}/src/*.cpp
)

# Add header files
file(GLOB_RECURSE HEADER_FILES
    ${CMAKE_SOURCE_DIR}/src/*.h
    ${CMAKE_SOURCE_DIR}/src/*.hpp
)

# Include directories
include_directories("${CMAKE_SOURCE_DIR}/include")
include_directories("${CMAKE_SOURCE_DIR}/include/imgui")
include_directories("${CMAKE_SOURCE_DIR}/extern/")
# include_directories("/usr/include/SDL3") # for dear imgui


# Define executable
add_executable("geophagia" ${SRC_FILES} ${HEADER_FILES})

# External dependencies
find_package(Threads REQUIRED)
find_package(OpenGL REQUIRED)
message(STATUS "Found OpenGL")
find_package(SDL3 REQUIRED)
message(STATUS "Found SDL3")
find_package(assimp REQUIRED)
message(STATUS "Found assimp")

# Set libraries
# GLAD
add_library(GLAD "extern/glad/glad.c")
message(STATUS "Glad is set")

# Imgui
file(GLOB_RECURSE IMGUI_SRC
    ${CMAKE_SOURCE_DIR}/extern/imgui/*.cpp
)
add_library(Imgui SHARED ${IMGUI_SRC})
message(STATUS "Imgui is set")



set(LIBS Necrosis GLAD SDL3 Imgui assimp)

# Linking libraries
target_link_libraries("geophagia" Threads::Threads ${LIBS})

add_subdirectory("extern/Necrosis")

