# options
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -stdlib=libc++ -std=c++1y")
set(CXX_FLAGS_DEBUG "-g -O0")
set(EXECUTABLE_NAME "tangram")

add_definitions(-DPLATFORM_OSX)

# load glfw
include(${PROJECT_SOURCE_DIR}/toolchains/add_glfw.cmake)

# load core library
add_subdirectory(${PROJECT_SOURCE_DIR}/core)
include_directories(${CORE_INCLUDE_DIRS})

# add sources and include headers
set(OSX_EXTENSIONS_FILES *.mm *.cpp)
foreach(_ext ${OSX_EXTENSIONS_FILES})
    find_sources_and_include_directories(
        ${PROJECT_SOURCE_DIR}/osx/src/*.h 
        ${PROJECT_SOURCE_DIR}/osx/src/${_ext})
endforeach()

# locate resource files to include
file(GLOB_RECURSE RESOURCES ${PROJECT_SOURCE_DIR}/osx/resources/**)
file(GLOB_RECURSE CORE_RESOURCES ${PROJECT_SOURCE_DIR}/core/resources/**)
list(APPEND RESOURCES ${CORE_RESOURCES})
string(REGEX REPLACE "[.]DS_Store" "" RESOURCES "${RESOURCES}")

# link and build functions
function(link_libraries)

    target_link_libraries(${EXECUTABLE_NAME} core glfw ${GLFW_LIBRARIES})

    # add resource files and property list
    set_target_properties(${EXECUTABLE_NAME} PROPERTIES
        MACOSX_BUNDLE_INFO_PLIST "${PROJECT_SOURCE_DIR}/osx/resources/tangram-Info.plist"
        RESOURCE "${RESOURCES}")

endfunction()

function(build) 

    add_executable(${EXECUTABLE_NAME} MACOSX_BUNDLE ${SOURCES} ${RESOURCES})

endfunction()
