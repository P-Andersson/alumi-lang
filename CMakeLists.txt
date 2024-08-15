cmake_minimum_required(VERSION 3.28)

project(alumi-lang)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_PROJECT_TOP_LEVEL_INCLUDES 
  "external/cmake-conan/conan_provider.cmake" # conan dependeny provider
CACHE STRING "Top level includes to use")

set(CMAKE_MODULE_PATH "${CMAKE_BINARY_DIR};${CMAKE_MODULE_PATH}")

enable_testing()

# Catch configuration
add_compile_definitions(CATCH_CONFIG_CPP17_UNCAUGHT_EXCEPTIONS)

add_subdirectory(lib)