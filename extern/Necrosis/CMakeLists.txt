# Add src files
file(GLOB_RECURSE SRC_FILES
    *.c
    *.cpp
)

# Add header files
file(GLOB_RECURSE HEADER_FILES
    *.h
    *.hpp
)

# Define include path
include_directories("${CMAKE_SOURCE_DIR}/include")
include_directories("${CMAKE_SOURCE_DIR}/include/imgui")

# External dependencies

# Set libraries

# Linking libraries
# set(CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_CXX_LINK_EXECUTABLE} -ldl")
# target_link_libraries(${PROJECT_NAME} ${LIBS})

# Create engine library
add_library(Necrosis SHARED ${SRC_FILES})
# add_library(Necrosis ${SRC_FILES})