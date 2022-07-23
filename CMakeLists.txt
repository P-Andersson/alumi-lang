project(vclang)

cmake_minimum_required(VERSION 3.22)

if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
  message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
  file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/0.18.1/conan.cmake"
                "${CMAKE_BINARY_DIR}/conan.cmake"
                TLS_VERIFY ON)
endif()

include(${CMAKE_BINARY_DIR}/conan.cmake)

conan_cmake_configure(REQUIRES 
    catch2/2.13.4
    fmt/9.0.0 
    utfcpp/3.2.1
GENERATORS cmake_find_package)

conan_cmake_autodetect(settings)

conan_cmake_install(PATH_OR_REFERENCE .
                    BUILD missing
                    REMOTE conancenter
                    SETTINGS ${settings})
set(CMAKE_MODULE_PATH "${CMAKE_BINARY_DIR};${CMAKE_MODULE_PATH}")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
 
enable_testing()

# Catch configuration
add_compile_definitions(CATCH_CONFIG_CPP17_UNCAUGHT_EXCEPTIONS)

add_subdirectory(lib)