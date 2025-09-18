cmake_minimum_required(VERSION 3.10)
project(hw1 VERSION 1.0)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# GLFW options - disable unnecessary features
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

# Add GLFW subdirectory to build it from source
add_subdirectory(deps/glfw-3.4)

# Create the executable
add_executable(hw1 src/main.cpp)

# Link libraries
target_link_libraries(hw1 
    glfw
    ${CMAKE_DL_LIBS}
)

# Platform-specific linking
if(WIN32)
    target_link_libraries(hw1 opengl32)
elseif(APPLE)
    find_library(OPENGL_LIBRARY OpenGL)
    find_library(COCOA_LIBRARY Cocoa)
    find_library(IOKIT_LIBRARY IOKit)
    find_library(COREVIDEO_LIBRARY CoreVideo)
    target_link_libraries(hw1 ${OPENGL_LIBRARY} ${COCOA_LIBRARY} ${IOKIT_LIBRARY} ${COREVIDEO_LIBRARY})
else()
    find_package(OpenGL REQUIRED)
    target_link_libraries(hw1 ${OPENGL_LIBRARIES})
endif()

# Include directories
target_include_directories(hw1 PRIVATE 
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/deps/glfw-3.4/include
    ${CMAKE_CURRENT_SOURCE_DIR}/deps/glfw-3.4/deps
)
