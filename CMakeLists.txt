cmake_minimum_required(VERSION 4.0)

project(ant_simulation VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SFML 3 CONFIG REQUIRED
    COMPONENTS
        Graphics
        Window
        System
        Audio
)

file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cxx"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hh"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hxx"
)

add_executable(ant_simulation ${PROJECT_SOURCES})

target_include_directories(ant_simulation PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(ant_simulation PRIVATE
    SFML::Graphics
    SFML::Window
    SFML::System
    SFML::Audio
)

set(RESOURCES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/resources")

if(APPLE)
    set_target_properties(ant_simulation PROPERTIES MACOSX_BUNDLE TRUE)

    file(GLOB_RECURSE RESOURCE_FILES CONFIGURE_DEPENDS "${RESOURCES_DIR}/*")
    foreach(RESOURCE_FILE ${RESOURCE_FILES})
        file(RELATIVE_PATH RESOURCE_REL_PATH "${RESOURCES_DIR}" "${RESOURCE_FILE}")
        get_filename_component(RESOURCE_REL_DIR "${RESOURCE_REL_PATH}" DIRECTORY)
        if(RESOURCE_REL_DIR STREQUAL "")
            set_source_files_properties("${RESOURCE_FILE}" PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
        else()
            set_source_files_properties("${RESOURCE_FILE}" PROPERTIES MACOSX_PACKAGE_LOCATION "Resources/${RESOURCE_REL_DIR}")
        endif()
    endforeach()

    target_sources(ant_simulation PRIVATE ${RESOURCE_FILES})
else()
    add_custom_command(TARGET ant_simulation POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${RESOURCES_DIR}" "$<TARGET_FILE_DIR:ant_simulation>/resources"
        COMMENT "Copying resources next to the executable"
    )
endif()