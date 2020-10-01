cmake_minimum_required (VERSION 3.0)

project (kcmlaptop)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_BUILD_WITH_INSTALL_RPATH ON)

find_package(KDE1 REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(libsystemd IMPORTED_TARGET libsystemd)

include_directories(common)

include(CMakePackageConfigHelpers)
include(Qt1Macros)
include(KDE1Macros)
include(KDE1InstallDirs)

option(ENABLE_SANITIZERS "Enable runtime sanitizing (for development)")
if (ENABLE_SANITIZERS)
    message("Enabling asan and ubsan")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined")
endif()

if (CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-write-strings")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive -std=c++98 -Wno-write-strings")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--no-undefined")
endif()

add_subdirectory(laptop)

