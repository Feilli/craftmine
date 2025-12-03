cmake_minimum_required(VERSION 3.10)

project(CraftMine)

set(CMAKE_CXX_STANDARD 23)

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Load Dependencies
include(Dependencies.cmake)

# Projects
add_subdirectory(Core)
add_subdirectory(App)