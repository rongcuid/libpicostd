cmake_minimum_required(VERSION 3.15)

project(picostd VERSION 0.1.0 LANGUAGES C)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# Dependencies
# Prefer the CMake package config that ships with hwloc (lowercase name).
find_package(hwloc QUIET CONFIG)

if(TARGET hwloc::hwloc)
    set(PICOSTD_HWLOC_TARGET hwloc::hwloc)
elseif(TARGET Hwloc::Hwloc)
    set(PICOSTD_HWLOC_TARGET Hwloc::Hwloc)
elseif(TARGET HWLOC::hwloc)
    set(PICOSTD_HWLOC_TARGET HWLOC::hwloc)
endif()

# Fallback: pkg-config (covers distros that don't ship CMake config files).
if(NOT DEFINED PICOSTD_HWLOC_TARGET)
    find_package(PkgConfig QUIET)
    if(PkgConfig_FOUND)
        pkg_check_modules(HWLOC REQUIRED IMPORTED_TARGET hwloc)
        if(TARGET PkgConfig::HWLOC)
            set(PICOSTD_HWLOC_TARGET PkgConfig::HWLOC)
        endif()
    endif()
endif()

if(NOT DEFINED PICOSTD_HWLOC_TARGET)
    message(FATAL_ERROR "Could not find hwloc. Install the hwloc development package (providing hwlocConfig.cmake or pkg-config 'hwloc').")
endif()

# Let callers choose static or shared builds via standard BUILD_SHARED_LIBS.
option(BUILD_SHARED_LIBS "Build shared libraries" ON)

set(PICOSTD_SOURCES
    src/picostd.c
    src/arena.c
    src/args.c
    src/conversion.c
    src/cpubind.c
    src/waitpkg.c
)

set(PICOSTD_PUBLIC_HEADERS
    src/arena.h
    src/args.h
    src/conversion.h
    src/cpubind.h
    src/errors.h
    src/picomath.h
    src/picostd.h
    src/waitpkg.h
)

add_library(picostd ${PICOSTD_SOURCES})
add_library(picostd::picostd ALIAS picostd)

# Use PIC so the static library can be linked into shared objects.
set_target_properties(picostd PROPERTIES
    POSITION_INDEPENDENT_CODE ON
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    OUTPUT_NAME picostd
)

target_compile_features(picostd PUBLIC c_std_11)

target_include_directories(picostd
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/picostd>
)

target_link_libraries(picostd
    PUBLIC
        ${PICOSTD_HWLOC_TARGET}
)

# Installation
install(TARGETS picostd
    EXPORT picostdTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/picostd
)

install(FILES ${PICOSTD_PUBLIC_HEADERS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/picostd
)

# CMake package configuration
configure_package_config_file(
    cmake/picostdConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/picostdConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/picostd
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/picostdConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(EXPORT picostdTargets
    FILE picostdTargets.cmake
    NAMESPACE picostd::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/picostd
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/picostdConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/picostdConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/picostd
)

# pkg-config file
configure_file(cmake/picostd.pc.in ${CMAKE_CURRENT_BINARY_DIR}/picostd.pc @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/picostd.pc
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
)

option(PICOSTD_ENABLE_TESTS "Enable building tests" OFF)
if(PICOSTD_ENABLE_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
