cmake_minimum_required(VERSION 3.14)

find_package(Threads)

add_library(orni INTERFACE)

add_library(orni::orni ALIAS orni)

target_compile_features(orni
        INTERFACE
            cxx_std_17
)

target_include_directories(orni
        INTERFACE
            $<INSTALL_INTERFACE:include>
            $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
)

target_link_libraries(orni
        INTERFACE
            Threads::Threads
)

## Installation shenanigans

install(
        TARGETS
            orni
        EXPORT
            orniTargets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
)

install(
        EXPORT orniTargets
        FILE orniTargets.cmake
        NAMESPACE orni::
        DESTINATION cmake
)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/orni
        DESTINATION include)

include(CMakePackageConfigHelpers)

write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/orniConfigVersion.cmake
        COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
        ${CMAKE_CURRENT_LIST_DIR}/orniConfig.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/orniConfig.cmake
        INSTALL_DESTINATION cmake
)

install(
        FILES
            ${CMAKE_CURRENT_BINARY_DIR}/orniConfig.cmake
            ${CMAKE_CURRENT_BINARY_DIR}/orniConfig.cmake
        DESTINATION cmake
)
