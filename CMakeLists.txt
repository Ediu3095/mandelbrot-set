cmake_minimum_required(VERSION 3.20..3.30)
project(
  mandelbrot-set
  VERSION 0.2.0
  DESCRIPTION "A real-time mandelbrot set renderer"
  LANGUAGES C CXX
)

set(CMAKE_CXX_STANDARD 20)

# Submodules
include(cmake/UpdateSubmodules.cmake)
add_subdirectory(submodules)

# Project files
add_subdirectory(${PROJECT_NAME})
