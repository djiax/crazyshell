cmake_minimum_required(VERSION 3.15)

project(minishell CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Explicitly list source files (Safer than file(GLOB))
set(SOURCES
    src/main.cpp
    src/Shell.cpp
)

# 1. Create target
add_executable(crazy ${SOURCES})

# 2. Add include path
target_include_directories(crazy PRIVATE include)
