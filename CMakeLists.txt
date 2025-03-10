cmake_minimum_required(VERSION 3.14)
project(external-control CXX)

set(CMAKE_CXX_STANDARD 17)

#######################################################################
# Main
#######################################################################
include_directories(include)
add_subdirectory(src)

#######################################################################
# GUI
#######################################################################
add_subdirectory(gui)