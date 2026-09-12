cmake_minimum_required(VERSION 3.15)
project(HeadCreateScript VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(hcs src/main.cpp)

# Provide the project version to the compiler so main.cpp can print it
target_compile_definitions(hcs PRIVATE HCS_VERSION="${PROJECT_VERSION}")
